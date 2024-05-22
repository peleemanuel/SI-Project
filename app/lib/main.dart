import 'package:flutter/material.dart';

void main() {
  runApp(MyApp());
}

class MyApp extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      home: HomeScreen(),
    );
  }
}

class HomeScreen extends StatelessWidget {
  final List<Map<String, dynamic>> plantData = [
    {
      'plant': 'Plant 1',
      'humidity': 55,
      'temperature': 22,
      'light': 1500,
      'soilMoisture': 35,
    },
    {
      'plant': 'Plant 2',
      'humidity': 60,
      'temperature': 23,
      'light': 1600,
      'soilMoisture': 33,
    },
    {
      'plant': 'Plant 3',
      'humidity': 58,
      'temperature': 21,
      'light': 1550,
      'soilMoisture': 34,
    },
    {
      'plant': 'Plant 4',
      'humidity': 57,
      'temperature': 24,
      'light': 1580,
      'soilMoisture': 36,
    },
  ];

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Smart Plant Greenhouse'),
      ),
      body: ListView.builder(
        itemCount: plantData.length,
        itemBuilder: (context, index) {
          final plant = plantData[index];
          return Card(
            child: Padding(
              padding: const EdgeInsets.all(8.0),
              child: Column(
                crossAxisAlignment: CrossAxisAlignment.start,
                children: [
                  Text(plant['plant'], style: TextStyle(fontSize: 18, fontWeight: FontWeight.bold)),
                  Text('Humidity: ${plant['humidity']}%'),
                  Text('Temperature: ${plant['temperature']}°C'),
                  Text('Light: ${plant['light']} lx'),
                  Text('Soil Moisture: ${plant['soilMoisture']}%'),
                ],
              ),
            ),
          );
        },
      ),
      floatingActionButton: Column(
        mainAxisAlignment: MainAxisAlignment.end,
        children: [
          FloatingActionButton(
            onPressed: () {
              // Logic for opening hatch
            },
            child: Icon(Icons.open_in_new),
            tooltip: 'Open Hatch',
          ),
          SizedBox(height: 16),
          FloatingActionButton(
            onPressed: () {
              // Logic for turning light on/off
            },
            child: Icon(Icons.lightbulb),
            tooltip: 'Turn Light On/Off',
          ),
          SizedBox(height: 16),
          FloatingActionButton(
            onPressed: () {
              Navigator.push(
                context,
                MaterialPageRoute(builder: (context) => AddDataScreen()),
              );
            },
            child: Icon(Icons.add),
            tooltip: 'Add Data',
          ),
        ],
      ),
    );
  }
}

class AddDataScreen extends StatelessWidget {
  final TextEditingController tempMinController = TextEditingController();
  final TextEditingController tempMaxController = TextEditingController();
  final TextEditingController humiditySoilMinController = TextEditingController();
  final TextEditingController humiditySoilMaxController = TextEditingController();
  final TextEditingController humidityAirMinController = TextEditingController();
  final TextEditingController humidityAirMaxController = TextEditingController();
  final TextEditingController lightController = TextEditingController();

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: Text('Add Data'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            TextField(
              controller: tempMinController,
              decoration: InputDecoration(labelText: 'Min Temperature'),
            ),
            TextField(
              controller: tempMaxController,
              decoration: InputDecoration(labelText: 'Max Temperature'),
            ),
            TextField(
              controller: humiditySoilMinController,
              decoration: InputDecoration(labelText: 'Min Soil Humidity'),
            ),
            TextField(
              controller: humiditySoilMaxController,
              decoration: InputDecoration(labelText: 'Max Soil Humidity'),
            ),
            TextField(
              controller: humidityAirMinController,
              decoration: InputDecoration(labelText: 'Min Air Humidity'),
            ),
            TextField(
              controller: humidityAirMaxController,
              decoration: InputDecoration(labelText: 'Max Air Humidity'),
            ),
            TextField(
              controller: lightController,
              decoration: InputDecoration(labelText: 'Light (lx)'),
            ),
            ElevatedButton(
              onPressed: () {
                // Code to save data
              },
              child: Text('Save Data'),
            ),
          ],
        ),
      ),
    );
  }
}
