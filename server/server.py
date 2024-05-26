from flask import Flask, request, render_template, jsonify
import psycopg2
from werkzeug.utils import secure_filename
import logging
from flask_cors import CORS  # Import the CORS package

app = Flask(__name__)
CORS(app)  # Enable CORS for the Flask app

# Configure logging
logging.basicConfig(level=logging.DEBUG,
                    format='%(asctime)s - %(levelname)s - %(message)s')

# Connect to the PostgreSQL database
def get_db_connection():
    conn = psycopg2.connect(host="localhost", dbname="postgres",
                            user="postgres", password="1234", port=5432)
    return conn

# Function to update database schema
def update_database_schema():
    try:
        conn = get_db_connection()
        cur = conn.cursor()
        cur.execute("""
            ALTER TABLE sera ADD COLUMN IF NOT EXISTS auto_mode_manual_mode BOOLEAN DEFAULT TRUE;
        """)
        conn.commit()
        cur.close()
        conn.close()
    except Exception as e:
        print("Error during database schema update:", e)

# Define a route to create tables if not exist and serve the form
@app.route('/')
def index():
    update_database_schema()  # Call the function to update the database schema
    try:
        conn = get_db_connection()
        cur = conn.cursor()
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
        soil_hum4 FLOAT,
        auto_mode_manual_mode BOOLEAN DEFAULT TRUE
    );
    """)
        conn.commit()
        cur.close()
        conn.close()
    except Exception as e:
        print("Error during database operation:", e)
    return render_template('index.html')

# Routes to handle sera data
@app.route('/add_sera_data', methods=['POST'])
def add_sera():
    data = request.get_json()
    print("Data received:", data)
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
    auto_mode_manual_mode = data.get('auto_mode_manual_mode', True)  # Default to True if not provided

    conn = get_db_connection()
    cur = conn.cursor()

    # Query pentru upsert
    cur.execute("""
        INSERT INTO sera (sera_id, light, temp_in, temp_out, hum_in, hum_out, soil_hum1, soil_hum2, soil_hum3, soil_hum4, auto_mode_manual_mode)
        VALUES (%s, %s, %s, %s, %s, %s, %s, %s, %s, %s, %s)
        ON CONFLICT (sera_id) DO UPDATE
        SET light = EXCLUDED.light,
            temp_in = EXCLUDED.temp_in,
            temp_out = EXCLUDED.temp_out,
            hum_in = EXCLUDED.hum_in,
            hum_out = EXCLUDED.hum_out,
            soil_hum1 = EXCLUDED.soil_hum1,
            soil_hum2 = EXCLUDED.soil_hum2,
            soil_hum3 = EXCLUDED.soil_hum3,
            soil_hum4 = EXCLUDED.soil_hum4,
            auto_mode_manual_mode = EXCLUDED.auto_mode_manual_mode;
    """, (sera_id, light, temp_in, temp_out, hum_in, hum_out, soil_hum1, soil_hum2, soil_hum3, soil_hum4, auto_mode_manual_mode))
    conn.commit()
    cur.close()
    conn.close()

    if auto_mode_manual_mode:
        print("The application has been set to Auto Mode")

    return jsonify({"message": "Data added successfully!"}), 201

#PUNETI VALORILE CARE TREBUIE AICI****************************************************************
@app.route('/get_status', methods=['GET'])
def get_status():
    status = {
        "plant_status": "healthy",
        "pest_status": "healthy"
    }
    print("Status data fetched successfully")
    return jsonify(status), 200

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
            'soil_hum4': row[9],
            'auto_mode_manual_mode': row[10]
        })

    print("Sera data fetched successfully")
    return jsonify(sera_data), 200

# Route to handle actions from buttons
@app.route('/action', methods=['POST'])
def handle_action():
    data = request.get_json()
    message = data.get('message')
    print(f"Action received: {message}")
    return jsonify({"message": f"Action {message} received!"}), 201

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

        if (photo):
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
