import 'dart:async' show StreamSubscription;

import 'package:flutter/material.dart';
import 'package:quick_blue/quick_blue.dart';

import '../settings/settings_view.dart';
import '../util.dart';
import 'device_detail.dart';

import 'package:signal_strength_indicator/signal_strength_indicator.dart';

/// Displays a list of SampleItems.
class DeviceListView extends StatefulWidget {
  const DeviceListView({super.key});

  static const routeName = '/';
  @override
  State<DeviceListView> createState() => _DeviceListView();
}

class _DeviceListView extends State<DeviceListView> {
  StreamSubscription<BlueScanResult>? _subscription;
  bool _scanning = false;

  final _scanResults = <BlueScanResult>[];
  @override
  void initState() {
    super.initState();
    _subscription = QuickBlue.scanResultStream.listen((result) {
      if (!_scanResults.any((r) => r.deviceId == result.deviceId)) {
        _scanResults.add(result);
        setState(() { _scanResults.sort((a,b) => b.rssi.compareTo(a.rssi)); });
      }
    });
  }
  @override
  void dispose() {
    super.dispose();
    _subscription?.cancel();
  }

  void _toggleScan() {
    if (_scanning) { QuickBlue.stopScan(); }
    else { QuickBlue.startScan(); }
    setState(() => _scanning = !_scanning);
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('CLOK - Select device'),
        actions: [
          IconButton(
            icon: const Icon(Icons.settings),
            onPressed: () {
              // Navigate to the settings page. If the user leaves and returns
              // to the app after it has been killed while running in the
              // background, the navigation stack is restored.
              Navigator.restorablePushNamed(context, SettingsView.routeName);
            },
          ),
        ],
      ),

      // To work with lists that may contain a large number of items, it’s best
      // to use the ListView.builder constructor.
      //
      // In contrast to the default ListView constructor, which requires
      // building all Widgets up front, the ListView.builder constructor lazily
      // builds Widgets as they’re scrolled into view.
      body: ListView.builder(
        // Providing a restorationId allows the ListView to restore the
        // scroll position when a user leaves and returns to the app after it
        // has been killed while running in the background.
        restorationId: 'deviceListView',
        itemCount: _scanResults.length,
        itemBuilder: (BuildContext context, int index) => ListTile(
          title:
              Text('${_scanResults[index].name}(${_scanResults[index].rssi})'),
          subtitle: Text('${stringToIntToMAC(_scanResults[index].deviceId)} (${_scanResults[index].deviceId})'),
          leading: SignalStrengthIndicator.bars(
            value: _scanResults[index].rssi,
            minValue: -120, maxValue: -10,
            size: 50, barCount: 4, spacing: 0.2,
          ),
          onTap: () {
            // stop scan if scanning
            if (_scanning) { _toggleScan(); }
            Navigator.pushNamed(context, DeviceDetailsView.routeName, arguments: _scanResults[index].deviceId);
        }),
      ),
      floatingActionButton: FutureBuilder(
        // Initialize FlutterFire:
        future: QuickBlue.isBluetoothAvailable(),
        builder: (context, snapshot) {
          bool ready = (snapshot.data ?? false);
          return FloatingActionButton(
            onPressed: ready ? _toggleScan : null ,
            tooltip: ready ? _scanning ? 'Stop Scan' : 'Start Scan' : 'Bluetooth not available. Restart App.',
            child: ready ? _scanning ? const Icon(Icons.search_off): const Icon(Icons.search) : const Icon(Icons.bluetooth_disabled),
            );
          }
      )
    );
  }
}
