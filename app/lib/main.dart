import 'package:flutter/material.dart';

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
      backgroundColor: Color(0xFF6699CC),
      appBar: AppBar(
        title: Text('Smart Plant Greenhouse'),
      ),
      body: Column(
        children: [
          Expanded(
            child: ListView.builder(
              itemCount: plantData.length,
              itemBuilder: (context, index) {
                final plant = plantData[index];
                return Card(
                  color: Color(0xFFA7C7E7), // Light green card color
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
          ),
          SizedBox(height: 16),
          Row(
            mainAxisAlignment: MainAxisAlignment.spaceEvenly,
            children: [
              FloatingActionButton.extended(
                onPressed: () {
                  // Logic for opening hatch
                },
                label: Text('Open Hatch', style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.open_in_new, color: Colors.white),
              ),
              FloatingActionButton.extended(
                onPressed: () {
                  // Logic for turning light on/off
                },
                label: Text('Turn On/Off Light', style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.lightbulb, color: Colors.white),
              ),
              FloatingActionButton.extended(
                onPressed: () {
                  Navigator.push(
                    context,
                    MaterialPageRoute(builder: (context) => AddDataScreen()),
                  );
                },
                label: Text('Add Flower', style: TextStyle(color: Colors.white)),
                icon: Icon(Icons.add, color: Colors.white),
              ),
            ],
          ),
          SizedBox(height: 16),
        ],
      ),
    );
  }
}

class AddDataScreen extends StatefulWidget {
  @override
  _AddDataScreenState createState() => _AddDataScreenState();
}

class _AddDataScreenState extends State<AddDataScreen> {
  int soilHumidity = 0;
  int temperature = 0;
  int light = 0;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Color(0xFF6699CC),
      appBar: AppBar(
        title: Text('Add Data'),
      ),
      body: Padding(
        padding: const EdgeInsets.all(16.0),
        child: Column(
          children: [
            Card(
              color: Color(0xFFA7C7E7), // Light green card color
              child: Padding(
                padding: const EdgeInsets.all(8.0),
                child: buildIncrementDecrementField('Soil Humidity', soilHumidity, (newValue) {
                  setState(() {
                    soilHumidity = newValue;
                  });
                }),
              ),
            ),
            SizedBox(height: 16),
            Card(
              color: Color(0xFFA7C7E7), // Light green card color
              child: Padding(
                padding: const EdgeInsets.all(8.0),
                child: buildIncrementDecrementField('Temperature', temperature, (newValue) {
                  setState(() {
                    temperature = newValue;
                  });
                }),
              ),
            ),
            SizedBox(height: 16),
            Card(
              color: Color(0xFFA7C7E7), // Light green card color
              child: Padding(
                padding: const EdgeInsets.all(8.0),
                child: buildIncrementDecrementField('Light', light, (newValue) {
                  setState(() {
                    light = newValue;
                  });
                }),
              ),
            ),
            SizedBox(height: 16),
            FloatingActionButton.extended(
              onPressed: () {
                // Code to save data
              },
              label: Text('Save Data', style: TextStyle(color: Colors.white)),
              icon: Icon(Icons.save, color: Colors.white),
              backgroundColor: Color(0xFF6699CC),
            ),
          ],
        ),
      ),
    );
  }

  Widget buildIncrementDecrementField(String label, int currentValue, ValueChanged<int> onChanged) {
    return Padding(
      padding: const EdgeInsets.symmetric(vertical: 8.0),
      child: Row(
        mainAxisAlignment: MainAxisAlignment.spaceBetween,
        children: [
          Text(label, style: TextStyle(fontSize: 16, color: Colors.black)),
          Row(
            children: [
              IconButton(
                onPressed: () {
                  if (currentValue > 0) onChanged(currentValue - 1);
                },
                icon: Icon(Icons.remove, color: Colors.black),
              ),
              Text('$currentValue', style: TextStyle(fontSize: 16, color: Colors.black)),
              IconButton(
                onPressed: () {
                  onChanged(currentValue + 1);
                },
                icon: Icon(Icons.add, color: Colors.black),
              ),
            ],
          ),
        ],
      ),
    );
  }
}
