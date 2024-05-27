import requests
import json

# Adresa IP a serverului ESP32 și endpoint-ul
url = 'http://192.168.1.154/command'

# Dicționarul cu datele care vor fi trimise
data = {
    "command": "toggleServo"
}

# Trimiterea requestului POST
try:
    response = requests.post(url, json=data)  # Folosim direct parametrul json= pentru a trimite dicționarul serializat
    # Verificăm dacă requestul a fost successful
    if response.status_code == 200:
        print("Request trimis cu succes!")
        print("Răspuns:", response.text)
    else:
        print("Eroare la trimiterea requestului, status code:", response.status_code)
except requests.exceptions.RequestException as e:
    print("Eroare la conexiune:", e)
