import fs from "fs";
import { spawn } from "child_process";

// Function to process image classification
const processImageClassification = async (imageName) => {
  try {
    const imagePath= "./images/" + imageName;
    // Load the image
    const image = fs.readFileSync(imagePath);

    // Execute Python script for classification
    const predictedClass = await executePythonScript(imagePath);

    return predictedClass;
  } catch (error) {
    console.error("Error processing image classification:", error);
    throw error;
  }
};

// Function to execute Python script for classification
const executePythonScript = async (imagePath) => {
  return new Promise((resolve, reject) => {
    // Path to your IPython notebook script for classification
    const pythonScript = "/plants_model/plants.ipynb";

    // Spawn a Python process to execute the script
    const pythonProcess = spawn("ipython", ["nbconvert", "--to", "python", pythonScript]);

    pythonProcess.stdout.on("data", (data) => {
      console.log(`stdout: ${data}`);
    });

    pythonProcess.stderr.on("data", (data) => {
      console.error(`stderr: ${data}`);
      reject(data);
    });

    pythonProcess.on("close", (code) => {
      console.log(`child process exited with code ${code}`);

      // Once the Python script is executed, you can read the output
      // and resolve the promise with the predicted class
      const predictedClass = "Class"; // Replace this with the actual predicted class
      resolve(predictedClass);
    });
  });
};

export default processImageClassification;
