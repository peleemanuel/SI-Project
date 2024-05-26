import fs from "fs";
import path from "path";
import { spawn } from "child_process";

// Function to execute Python script for classification
const executePythonScript = async (imagePath, pythonScript) => {
  return new Promise((resolve, reject) => {
    // Spawn a Python process to execute the script
    const pythonProcess = spawn("python", [pythonScript, imagePath]);

    let output = '';
    pythonProcess.stdout.on("data", (data) => {
      output += data.toString();
    });

    pythonProcess.stderr.on("data", (data) => {
      console.error(`stderr: ${data}`);
      reject(data.toString());
    });

    pythonProcess.on("close", (code) => {
      if (code !== 0) {
        reject(`Python script exited with code ${code}`);
      } else {
        resolve(output.trim());
      }
    });
  });
};

// Function to process image classification
const processImageClassification = async (imageName, pythonScript) => {
  try {
    const imagePath = path.join(__dirname, 'images', imageName);

    // Ensure the image exists
    if (!fs.existsSync(imagePath)) {
      throw new Error(`Image not found: ${imagePath}`);
    }

    // Execute Python script for classification
    const predictedClass = await executePythonScript(imagePath, pythonScript);

    return predictedClass;
  } catch (error) {
    console.error("Error processing image classification:", error);
    throw error;
  }
};

export default processImageClassification;
