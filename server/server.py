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
      #  cur.execute("""
      #             TRUNCATE TABLE photos;
      #              """)
        cur.execute("""
        CREATE TABLE IF NOT EXISTS photos (
            photo_id SERIAL PRIMARY KEY,
            photo_name VARCHAR(255),
            photo_data BYTEA
        );
    """)
        cur.execute("""
    CREATE TABLE IF NOT EXISTS sera (
        sera_id SERIAL PRIMARY KEY,
        light FLOAT,
        temp_in FLOAT,
        temp_out FLOAT,
        hum_in FLOAT,
        hum_out FLOAT,
        soil_hum1 FLOAT,
        soil_hum2 FLOAT,
        soil_hum3 FLOAT,
        soil_hum4 FLOAT
    );
    """)
        conn.commit()
        cur.close()
        conn.close()
    except Exception as e:
        print("Error during database operation:", e)
    return render_template('index.html')

#Routes ti handle sera date
@app.route('/add_sera_data', methods=['POST'])
def add_sera():
    data = request.get_json()
    sera_id = data.get('sera_id')
    light = data.get('light')
    temp_in = data.get('temp_in')
    temp_out = data.get('temp_out')
    hum_in = data.get('hum_in')
    hum_out = data.get('hum_out')
    soil_hum1 = data.get('soil_hum1')
    soil_hum2 = data.get('soil_hum2')
    soil_hum3 = data.get('soil_hum3')
    soil_hum4 = data.get('soil_hum4')

    
    conn = get_db_connection()
    cur = conn.cursor()
    
    cur.execute("""
        UPDATE sera
        SET light = %s, temp_in = %s, temp_out = %s, hum_in = %s, hum_out = %s, soil_hum1 = %s, soil_hum2 = %s, soil_hum3 = %s, soil_hum4 = %s
        WHERE sera_id = %s
    """, (light, temp_in, temp_out, hum_in, hum_out, soil_hum1, soil_hum2, soil_hum3, soil_hum4, sera_id))
    
    conn.commit()
    cur.close()
    conn.close()
    
    return jsonify({"message": "Data added successfully!"}), 201  

@app.route('/get_sera_data', methods=['GET'])
def get_sera_data():
    conn = get_db_connection()
    cur = conn.cursor()
    
    cur.execute("SELECT * FROM sera")
    rows = cur.fetchall()
    
    cur.close()
    conn.close()
    
    sera_data = []
    for row in rows:
        sera_data.append({
            'sera_id': row[0],
            'light': row[1],
            'temp_in': row[2],
            'temp_out': row[3],
            'hum_in': row[4],
            'hum_out': row[5],
            'soil_hum1': row[6],
            'soil_hum2': row[7],
            'soil_hum3': row[8],
            'soil_hum4': row[9]
        })
    
    return jsonify(sera_data), 200

# Route to handle file uploads ---> photo
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
