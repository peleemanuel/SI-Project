import 'package:flutter/material.dart';
import 'dart:convert';
import 'package:http/http.dart' as http;
import 'dart:async'; // Importă Timer

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      theme: ThemeData(
        primarySwatch: Colors.blue,
        floatingActionButtonTheme: FloatingActionButtonThemeData(
          backgroundColor: Color(0xFF6699CC),
        ),
        appBarTheme: AppBarTheme(
          backgroundColor: Color(0xFF6699CC),
          titleTextStyle: TextStyle(
            color: Colors.white,
            fontWeight: FontWeight.bold,
            fontSize: 20,
          ),
        ),
      ),
      home: HomeScreen(),
    );
  }
}

class HomeScreen extends StatefulWidget {
  @override
  _HomeScreenState createState() => _HomeScreenState();
}

class _HomeScreenState extends State<HomeScreen> {
  List<Map<String, dynamic>> plantData = [];
  bool isLoading = true;
  Timer? timer; // Definește un Timer

  @override
  void initState() {
    super.initState();
    fetchPlantData();
    timer = Timer.periodic(Duration(seconds: 20), (Timer t) => fetchPlantData()); // Setează Timer-ul pentru a face fetch la date la fiecare 20 de secunde
  }

  @override
  void dispose() {
    timer?.cancel(); // Anulează Timer-ul când ecranul este închis
    super.dispose();
  }

  Future<void> fetchPlantData() async {
    try {
      final response = await http.get(Uri.parse('http://192.168.1.115:5000/get_sera_data'));

      if (response.statusCode == 200) {
        print('Data received: ${response.body}');
        setState(() {
          plantData = List<Map<String, dynamic>>.from(json.decode(response.body));
          // Ensure auto_mode_manual_mode is not null
          for (var plant in plantData) {
            plant['auto_mode_manual_mode'] ??= true;
          }
          isLoading = false;
        });
      } else {
        throw Exception('Failed to load data');
      }
    } catch (error) {
      setState(() {
        isLoading = false;
      });
      print('Error fetching data: $error');
    }
  }

  Future<void> updateAutoMode(bool value) async {
    if (plantData.isNotEmpty) {
      final url = Uri.parse('http://192.168.1.115:5000/add_sera_data');
      final response = await http.post(
        url,
        headers: <String, String>{
          'Content-Type': 'application/json; charset=UTF-8',
        },
        body: jsonEncode(<String, dynamic>{
          'sera_id': plantData[0]['sera_id'],
          'light': plantData[0]['light'],
          'temp_in': plantData[0]['temp_in'],
          'temp_out': plantData[0]['temp_out'],
          'hum_in': plantData[0]['hum_in'],
          'hum_out': plantData[0]['hum_out'],
          'soil_hum1': plantData[0]['soil_hum1'],
          'soil_hum2': plantData[0]['soil_hum2'],
          'soil_hum3': plantData[0]['soil_hum3'],
          'soil_hum4': plantData[0]['soil_hum4'],
          'auto_mode_manual_mode': value,
        }),
      );

      if (response.statusCode == 201) {
        setState(() {
          plantData[0]['auto_mode_manual_mode'] = value;
        });
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Mode updated successfully!')),
        );
      } else {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(content: Text('Failed to update mode!')),
        );
      }
    }
  }

  Future<void> sendRequest(String message) async {
    final url = Uri.parse('http://192.168.1.115:5000/action');
    final response = await http.post(
      url,
      headers: <String, String>{
        'Content-Type': 'application/json; charset=UTF-8',
      },
      body: jsonEncode(<String, String>{
        'message': message,
      }),
    );

    if (response.statusCode == 201) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Action sent: $message')),
      );
    } else {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Failed to send action')),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Color(0xFF6699CC),
      appBar: AppBar(
        title: Text('Smart Plant Greenhouse'),
      ),
      body: isLoading
          ? Center(child: CircularProgressIndicator())
          : plantData.isEmpty
          ? Center(child: Text('No data available'))
          : ListView.builder(
        itemCount: plantData.length,
        itemBuilder: (context, index) {
          final plant = plantData[index];
          bool isAutoMode = plant['auto_mode_manual_mode'] ?? true;
          return Card(
            color: Color(0xFFA7C7E7),
            child: Padding(
              padding: const EdgeInsets.all(8.0),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text('Sera ${plant['sera_id']}', style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold)),
                  Text('Light: ${plant['light']}'),
                  Text('Temp In: ${plant['temp_in']}°C'),
                  Text('Temp Out: ${plant['temp_out']}°C'),
                  Text('Humidity In: ${plant['hum_in']}%'),
                  Text('Humidity Out: ${plant['hum_out']}%'),
                  Text('Soil Humidity 1: ${plant['soil_hum1']}%'),
                  Text('Soil Humidity 2: ${plant['soil_hum2']}%'),
                  Text('Soil Humidity 3: ${plant['soil_hum3']}%'),
                  Text('Soil Humidity 4: ${plant['soil_hum4']}%'),
                  Row(
                    mainAxisAlignment: MainAxisAlignment.spaceBetween,
                    children: [
                      Text(isAutoMode ? 'Auto Mode' : 'Manual Mode', style: TextStyle(fontSize: 16)),
                      Switch(
                        value: isAutoMode,
                        onChanged: (value) {
                          updateAutoMode(value);
                        },
                      ),
                    ],
                  ),
                ],
              ),
            ),
          );
        },
      ),
      floatingActionButton: Column(
        mainAxisAlignment: MainAxisAlignment.end,
        children: [
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              FloatingActionButton.extended(
                onPressed: plantData.isNotEmpty && !plantData[0]['auto_mode_manual_mode']
                    ? () {
                  sendRequest('Open Hatch');
                }
                    : null,
                label: Text('Open Hatch', style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.open_in_new, color: Colors.white),
                backgroundColor: plantData.isNotEmpty && !plantData[0]['auto_mode_manual_mode'] ? Color(0xFF6699CC) : Colors.grey,
              ),
              FloatingActionButton.extended(
                onPressed: plantData.isNotEmpty && !plantData[0]['auto_mode_manual_mode']
                    ? () {
                  sendRequest('Turn On/Off Light');
                }
                    : null,
                label: Text('Turn On/Off Light', style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.lightbulb, color: Colors.white),
                backgroundColor: plantData.isNotEmpty && !plantData[0]['auto_mode_manual_mode'] ? Color(0xFF6699CC) : Colors.grey,
              ),
            ],
          ),
          SizedBox(height: 16),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              FloatingActionButton.extended(
                onPressed: plantData.isNotEmpty && !plantData[0]['auto_mode_manual_mode']
                    ? () {
                  sendRequest('Water Plant 1');
                }
                    : null,
                label: Text('Water Plant 1', style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.water_damage, color: Colors.white),
                backgroundColor: plantData.isNotEmpty && !plantData[0]['auto_mode_manual_mode'] ? Color(0xFF6699CC) : Colors.grey,
              ),
              FloatingActionButton.extended(
                onPressed: plantData.isNotEmpty && !plantData[0]['auto_mode_manual_mode']
                    ? () {
                  sendRequest('Water Plant 2');
                }
                    : null,
                label: Text('Water Plant 2', style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.water_damage, color: Colors.white),
                backgroundColor: plantData.isNotEmpty && !plantData[0]['auto_mode_manual_mode'] ? Color(0xFF6699CC) : Colors.grey,
              ),
            ],
          ),
          SizedBox(height: 16),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              FloatingActionButton.extended(
                onPressed: plantData.isNotEmpty && !plantData[0]['auto_mode_manual_mode']
                    ? () {
                  sendRequest('Water Plant 3');
                }
                    : null,
                label: Text('Water Plant 3', style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.water_damage, color: Colors.white),
                backgroundColor: plantData.isNotEmpty && !plantData[0]['auto_mode_manual_mode'] ? Color(0xFF6699CC) : Colors.grey,
              ),
              FloatingActionButton.extended(
                onPressed: plantData.isNotEmpty && !plantData[0]['auto_mode_manual_mode']
                    ? () {
                  sendRequest('Water Plant 4');
                }
                    : null,
                label: Text('Water Plant 4', style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.water_damage, color: Colors.white),
                backgroundColor: plantData.isNotEmpty && !plantData[0]['auto_mode_manual_mode'] ? Color(0xFF6699CC) : Colors.grey,
              ),
            ],
          ),
          SizedBox(height: 16),
        ],
      ),
    );
  }
}
