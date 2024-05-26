import 'package:flutter/material.dart';
import 'dart:convert';
import 'package:http/http.dart' as http;
import 'dart:async';

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

class _HomeScreenState extends State<HomeScreen> with WidgetsBindingObserver {
  List<Map<String, dynamic>> plantData = [];
  Map<String, dynamic> statusData = {
    "plant_status": "unknown",
    "pest_status": "unknown"
  };
  bool isLoading = true;
  Timer? timer;
  bool isHatchOpen = false; // Track the state of the hatch
  bool isLightOn = false; // Track the state of the light

  @override
  void initState() {
    super.initState();
    WidgetsBinding.instance.addObserver(this);
    fetchData();
    timer = Timer.periodic(Duration(seconds: 20), (Timer t) {
      fetchData();
    });
  }

  @override
  void dispose() {
    WidgetsBinding.instance.removeObserver(this);
    timer?.cancel();
    sendAutoModeRequest();
    super.dispose();
  }

  @override
  void didChangeAppLifecycleState(AppLifecycleState state) {
    if (state == AppLifecycleState.paused) {
      sendAutoModeRequest();
    }
  }

  Future<void> fetchData() async {
    await fetchPlantData();
    await fetchStatusData();
  }

  Future<void> fetchPlantData() async {
    try {
      final response =
          await http.get(Uri.parse('http://localhost:5000/get_sera_data'));

      if (response.statusCode == 200) {
        setState(() {
          plantData =
              List<Map<String, dynamic>>.from(json.decode(response.body));
          for (var plant in plantData) {
            plant['auto_mode_manual_mode'] ??= true;
          }
          isLoading = false;
        });
        print("Plant data fetched successfully");
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

  Future<void> fetchStatusData() async {
    try {
      final response =
          await http.get(Uri.parse('http://localhost:5000/get_status'));

      if (response.statusCode == 200) {
        setState(() {
          statusData = json.decode(response.body);
        });
        print("Status data fetched successfully");
      } else {
        throw Exception('Failed to load status');
      }
    } catch (error) {
      print('Error fetching status: $error');
    }
  }

  Future<void> updateAutoMode(bool value) async {
    if (plantData.isNotEmpty) {
      final url = Uri.parse('http://localhost:5000/add_sera_data');
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
        sendModeChangeMessage(value);
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

  Future<void> sendModeChangeMessage(bool isAutoMode) async {
    final url = Uri.parse('http://localhost:5000/action');
    final response = await http.post(
      url,
      headers: <String, String>{
        'Content-Type': 'application/json; charset=UTF-8',
      },
      body: jsonEncode(<String, String>{
        'message': isAutoMode ? '65' : '64', //auto mode: 65; manual mode: 64
      }),
    );

    if (response.statusCode == 201) {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
            content:
                Text('Mode change sent: ${isAutoMode ? 'Auto' : 'Manual'}')),
      );
    } else {
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Failed to send mode change message')),
      );
    }
  }

  Future<void> sendRequest(String message) async {
    final url = Uri.parse('http://localhost:5000/action');
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

  Future<void> sendAutoModeRequest() async {
    if (plantData.isNotEmpty) {
      final url = Uri.parse('http://localhost:5000/add_sera_data');
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
          'auto_mode_manual_mode': true, // Set mode to auto
        }),
      );

      if (response.statusCode == 201) {
        print('65'); //treci in auto mode
      } else {
        print('Failed to switch to Auto Mode before closing');
      }
    }
  }

  void toggleHatch() {
    setState(() {
      isHatchOpen = !isHatchOpen;
    });
    sendRequest(isHatchOpen ? '129' : '128'); //129-open trapa  128-close trapa
  }

  void toggleLight() {
    setState(() {
      isLightOn = !isLightOn;
    });
    sendRequest(isLightOn ? '193' : '193'); //turn on off led
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
                            Text('Sera ${plant['sera_id']}',
                                style: TextStyle(
                                    fontSize: 18, fontWeight: FontWeight.bold)),
                            Text('Light: ${plant['light']}'),
                            Text('Temp In: ${plant['temp_in']}°C'),
                            Text('Temp Out: ${plant['temp_out']}°C'),
                            Text('Humidity In: ${plant['hum_in']}%'),
                            Text('Humidity Out: ${plant['hum_out']}%'),
                            Text('Soil Humidity 1: ${plant['soil_hum1']}%'),
                            Text('Soil Humidity 2: ${plant['soil_hum2']}%'),
                            Text('Soil Humidity 3: ${plant['soil_hum3']}%'),
                            Text('Soil Humidity 4: ${plant['soil_hum4']}%'),
                            Text(
                                'Plant Status: ${statusData['plant_status']}'), // Afișează statusul plantei
                            Text(
                                'Pest Status: ${statusData['pest_status']}'), // Afișează statusul dăunătorilor
                            Row(
                              mainAxisAlignment: MainAxisAlignment.spaceBetween,
                              children: [
                                Text(isAutoMode ? 'Auto mode' : 'Manual mode',
                                    style: TextStyle(
                                        fontSize:
                                            16)), //65 automode  --- 64 manual mode
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
                onPressed: plantData.isNotEmpty &&
                        !plantData[0]['auto_mode_manual_mode']
                    ? toggleHatch
                    : null,
                label: Text(isHatchOpen ? 'Close hatch' : 'Open hatch',
                    style: TextStyle(
                        color: Colors
                            .white)), //128-deschide trapa, 129 inchide trapa
                icon: Icon(isHatchOpen ? Icons.close : Icons.open_in_new,
                    color: Colors.white),
                backgroundColor: plantData.isNotEmpty &&
                        !plantData[0]['auto_mode_manual_mode']
                    ? Color(0xFF6699CC)
                    : Colors.grey,
              ),
              FloatingActionButton.extended(
                onPressed: plantData.isNotEmpty &&
                        !plantData[0]['auto_mode_manual_mode']
                    ? toggleLight
                    : null,
                label: Text(isLightOn ? 'Turn off light' : 'Turn on light',
                    style: TextStyle(color: Colors.white)), //on/off lumina
                icon: Icon(
                    isLightOn ? Icons.lightbulb_outline : Icons.lightbulb,
                    color: Colors.white),
                backgroundColor: plantData.isNotEmpty &&
                        !plantData[0]['auto_mode_manual_mode']
                    ? Color(0xFF6699CC)
                    : Colors.grey,
              ),
            ],
          ),
          SizedBox(height: 16),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              FloatingActionButton.extended(
                onPressed: plantData.isNotEmpty &&
                        !plantData[0]['auto_mode_manual_mode']
                    ? () {
                        sendRequest('194'); //Water Plant 1
                      }
                    : null,
                label: Text('Water Plant 1',
                    style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.water_damage, color: Colors.white),
                backgroundColor: plantData.isNotEmpty &&
                        !plantData[0]['auto_mode_manual_mode']
                    ? Color(0xFF6699CC)
                    : Colors.grey,
              ),
              FloatingActionButton.extended(
                onPressed: plantData.isNotEmpty &&
                        !plantData[0]['auto_mode_manual_mode']
                    ? () {
                        sendRequest('196'); //Water Plant 2
                      }
                    : null,
                label: Text('Water Plant 2',
                    style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.water_damage, color: Colors.white),
                backgroundColor: plantData.isNotEmpty &&
                        !plantData[0]['auto_mode_manual_mode']
                    ? Color(0xFF6699CC)
                    : Colors.grey,
              ),
            ],
          ),
          SizedBox(height: 16),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              FloatingActionButton.extended(
                onPressed: plantData.isNotEmpty &&
                        !plantData[0]['auto_mode_manual_mode']
                    ? () {
                        sendRequest('200'); //Water Plant 3
                      }
                    : null,
                label: Text('Water Plant 3',
                    style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.water_damage, color: Colors.white),
                backgroundColor: plantData.isNotEmpty &&
                        !plantData[0]['auto_mode_manual_mode']
                    ? Color(0xFF6699CC)
                    : Colors.grey,
              ),
              FloatingActionButton.extended(
                onPressed: plantData.isNotEmpty &&
                        !plantData[0]['auto_mode_manual_mode']
                    ? () {
                        sendRequest('208'); //Water Plant 4
                      }
                    : null,
                label: Text('Water Plant 4',
                    style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.water_damage, color: Colors.white),
                backgroundColor: plantData.isNotEmpty &&
                        !plantData[0]['auto_mode_manual_mode']
                    ? Color(0xFF6699CC)
                    : Colors.grey,
              ),
            ],
          ),
          SizedBox(height: 16),
        ],
      ),
    );
  }
}
