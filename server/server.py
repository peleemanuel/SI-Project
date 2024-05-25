from flask import Flask, request, render_template, send_from_directory, jsonify
import os
import psycopg2
from werkzeug.utils import secure_filename
import logging

app = Flask(__name__)
# Configure logging
logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s - %(levelname)s - %(message)s')
# Connect to the PostgreSQL database


def get_db_connection():
    conn = psycopg2.connect(host="localhost", dbname="postgres",
                            user="postgres", password="1234", port=5432)
    return conn

# Define a route to create tables if not exist and serve the form


@app.route('/')
def index():
    try:
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute("""
                    TRUNCATE TABLE photos;
                    """)
        cur.execute("""
        CREATE TABLE IF NOT EXISTS photos (
            photo_id SERIAL PRIMARY KEY,
            photo_name VARCHAR(255),
            photo_data BYTEA
        );
    """)
        conn.commit()
        cur.close()
        conn.close()
    except Exception as e:
        print("Error during database operation:", e)
    return render_template('index.html')

# Route to handle file uploads


@app.route('/upload', methods=['POST'])
def upload_file():

    try:
        if 'file' not in request.files:
            return 'No file part'
        file = request.files['file']

        if file.filename == '':
            return 'No selected file'
        if file:
            filename = secure_filename(file.filename)
            print("filename: ", filename)
            filepath = os.path.join('images', filename)
            file.save(filepath)
            file_data = open(filepath, 'rb').read()

            # Insert file information into the database
            conn = get_db_connection()
            cur = conn.cursor()
            cur.execute("INSERT INTO photos (photo_name, photo_data) VALUES (%s, %s)",
                        (filename, file_data))
            conn.commit()
            cur.close()
            conn.close()
    except Exception as e:
        print("Error during database operation:", e)
    return jsonify({'message': 'File uploaded successfully'}), 200


@app.route('/upload2', methods=['POST'])
def handle_image_upload():
    image_data = request.data  # Datele imaginii, un stream de bytes
    with open('received_image.jpeg', 'wb') as f:
        f.write(image_data)  # Salvează imaginea pe disk
    return 'Image received and saved', 200


@app.route('/download_first_photo')
def download_first_photo():
    try:
        # Connect to the database
        conn = get_db_connection()
        cur = conn.cursor()

        # Execute a query to get the first photo
        cur.execute("SELECT photo_data FROM photos LIMIT 1")
        photo = cur.fetchone()

        if photo:
            # Save the photo data to a file
            with open('bbldrizzy.jpg', 'wb') as file:
                file.write(photo[0])

            cur.close()
            conn.close()
            return jsonify({'message': 'Photo downloaded successfully'}), 200
        else:
            return 'No photo found', 404
    except Exception as e:
        return f'An error occurred: {str(e)}', 500


if __name__ == '__main__':
    app.run(host='0.0.0.0', port=5000, debug=True)
