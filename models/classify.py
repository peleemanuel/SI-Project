import absl.logging
import torch.nn.functional as F
import torch.nn as nn
import torchvision.transforms as transforms
import torch
import logging
import json
import os
import numpy as np
from PIL import Image

# Suprimă mesajele de informare și avertizare ale TensorFlow
os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
logging.getLogger('tensorflow').setLevel(logging.ERROR)

# Suprimă mesajele de informare de la absl
absl.logging.set_verbosity(absl.logging.ERROR)

try:
    import tflite_runtime.interpreter as tflite
except ImportError:
    from tensorflow import lite as tflite


EXPORT_MODEL_VERSION = 1


def accuracy(outputs, labels):
    _, preds = torch.max(outputs, dim=1)
    return torch.tensor(torch.sum(preds == labels).item() / len(preds))


class ImageClassificationBase(nn.Module):
    def training_step(self, batch):
        images, labels = batch
        out = self(images)
        loss = F.cross_entropy(out, labels)
        return loss

    def validation_step(self, batch):
        images, labels = batch
        out = self(images)
        loss = F.cross_entropy(out, labels)
        acc = accuracy(out, labels)
        return {'val_loss': loss.detach(), 'val_acc': acc}

    def validation_epoch_end(self, outputs):
        batch_losses = [x['val_loss'] for x in outputs]
        epoch_loss = torch.stack(batch_losses).mean()
        batch_accs = [x['val_acc'] for x in outputs]
        epoch_acc = torch.stack(batch_accs).mean()
        return {'val_loss': epoch_loss.item(), 'val_acc': epoch_acc.item()}

    def epoch_end(self, epoch, result):
        print(f"Epoch [{epoch}], train_loss: {result['train_loss']:.4f}, val_loss: {result['val_loss']:.4f}, val_acc: {result['val_acc']:.4f}")


def ConvBlock(in_channels, out_channels, pool=False):
    layers = [nn.Conv2d(in_channels, out_channels, kernel_size=3, padding=1),
              nn.BatchNorm2d(out_channels),
              nn.ReLU(inplace=True)]
    if pool:
        layers.append(nn.MaxPool2d(4))
    return nn.Sequential(*layers)


class CNN_NeuralNet(ImageClassificationBase):
    def __init__(self, in_channels, num_diseases):
        super().__init__()
        self.conv1 = ConvBlock(in_channels, 64)
        self.conv2 = ConvBlock(64, 128, pool=True)
        self.res1 = nn.Sequential(ConvBlock(128, 128), ConvBlock(128, 128))
        self.conv3 = ConvBlock(128, 256, pool=True)
        self.conv4 = ConvBlock(256, 512, pool=True)
        self.res2 = nn.Sequential(ConvBlock(512, 512), ConvBlock(512, 512))
        self.classifier = nn.Sequential(nn.MaxPool2d(4),
                                        nn.Flatten(),
                                        nn.Linear(512, num_diseases))

    def forward(self, x):
        out = self.conv1(x)
        out = self.conv2(out)
        out = self.res1(out) + out
        out = self.conv3(out)
        out = self.conv4(out)
        out = self.res2(out) + out
        out = self.classifier(out)
        return out


def to_device(data, device):
    if isinstance(data, (list, tuple)):
        return [to_device(x, device) for x in data]
    return data.to(device, non_blocking=True)


def get_default_device():
    if torch.cuda.is_available():
        return torch.device("cuda")
    else:
        return torch.device("cpu")


device = get_default_device()


def load_model(model_path):
    model = torch.load(model_path)
    return model


with open('./classes.txt', 'r') as file:
    classes = file.readlines()

classes = [label.strip() for label in classes]

plants_model = to_device(CNN_NeuralNet(3, len(classes)), device)
plants_model.load_state_dict(torch.load('./plants.pth'))


def preprocess_image(image_path):
    transform = transforms.Compose([
        transforms.Resize((256, 256)),
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.485, 0.456, 0.406], std=[
                             0.229, 0.224, 0.225])
    ])
    image = Image.open(image_path)
    image = transform(image)
    return image


def predict_image(img, model):
    xb = to_device(img.unsqueeze(0), device)
    yb = model(xb)
    _, preds = torch.max(yb, dim=1)
    return classes[preds[0].item()]


class TFLiteModel:
    def __init__(self, dir_path):
        model_dir = os.path.dirname(dir_path)
        with open(os.path.join(model_dir, "signature.json"), "r") as f:
            self.signature = json.load(f)
        self.model_file = os.path.join(
            model_dir, self.signature.get("filename"))
        if not os.path.isfile(self.model_file):
            raise FileNotFoundError(f"Model file does not exist")
        self.interpreter = None
        self.signature_inputs = self.signature.get("inputs")
        self.signature_outputs = self.signature.get("outputs")
        version = self.signature.get("export_model_version")
        if version is None or version != EXPORT_MODEL_VERSION:
            print(
                f"Please use a model with a signature 'export_model_version' that matches {EXPORT_MODEL_VERSION}.")

    def load(self):
        self.interpreter = tflite.Interpreter(model_path=self.model_file)
        self.interpreter.allocate_tensors()
        input_details = {detail.get(
            "name"): detail for detail in self.interpreter.get_input_details()}
        self.model_inputs = {key: {
            **sig, **input_details.get(sig.get("name"))} for key, sig in self.signature_inputs.items()}
        output_details = {detail.get(
            "name"): detail for detail in self.interpreter.get_output_details()}
        self.model_outputs = {key: {
            **sig, **output_details.get(sig.get("name"))} for key, sig in self.signature_outputs.items()}
        if "Image" not in self.model_inputs:
            raise ValueError(
                "Tensorflow Lite model doesn't have 'Image' input! Check signature.json, and please report issue to Lobe.")

    def predict(self, image):
        if self.interpreter is None:
            self.load()
        input_data = self.process_image(
            image, self.model_inputs.get("Image").get("shape"))
        self.interpreter.set_tensor(
            self.model_inputs.get("Image").get("index"), input_data)
        self.interpreter.invoke()
        outputs = {key: self.interpreter.get_tensor(value.get("index")).tolist()[
            0] for key, value in self.model_outputs.items()}
        return self.process_output(outputs)

    def process_image(self, image, input_shape):
        width, height = image.size
        if image.mode != "RGB":
            image = image.convert("RGB")
        if width != height:
            square_size = min(width, height)
            left = (width - square_size) / 2
            top = (height - square_size) / 2
            right = (width + square_size) / 2
            bottom = (height + square_size) / 2
            image = image.crop((left, top, right, bottom))
        input_width, input_height = input_shape[1:3]
        if image.width != input_width or image.height != input_height:
            image = image.resize((input_width, input_height))
        image = np.asarray(image) / 255.0
        return image.reshape(input_shape).astype(np.float32)

    def process_output(self, outputs):
        out_keys = ["label", "confidence"]
        for key, val in outputs.items():
            if isinstance(val, bytes):
                outputs[key] = val.decode()
        confs = list(outputs.values())[0]
        labels = self.signature.get("classes").get("Label")
        output = [dict(zip(out_keys, group)) for group in zip(labels, confs)]
        sorted_output = {"predictions": sorted(
            output, key=lambda k: k["confidence"], reverse=True)}
        return sorted_output


def process_and_predict_image(image_path, plants_model, pests_model):
    image = preprocess_image(image_path)
    plant_prediction = predict_image(image, plants_model)

    image_pil = Image.open(image_path)
    pest_prediction = pests_model.predict(image_pil)
    pest_label = pest_prediction['predictions'][0]['label']
    pest_confidence = pest_prediction['predictions'][0]['confidence']

    if pest_confidence < 0.80:
        pest_label = "healthy"

    print(
        f"Plant Prediction - {plant_prediction.split('___')[1]}\n Pest Prediction - {pest_label}")


if __name__ == "__main__":

    pests_model = TFLiteModel(dir_path='.')
    pests_model.load()

    folder_path = '../server/'
    for filename in os.listdir(folder_path):
        if filename.lower().endswith(('.jpeg', '.jpg', '.png', '.bmp')):
            image_path = os.path.join(folder_path, filename.lower())
            process_and_predict_image(image_path, plants_model, pests_model)
