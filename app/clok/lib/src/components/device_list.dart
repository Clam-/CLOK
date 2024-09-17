import 'dart:async' show StreamSubscription;

import 'package:flutter/material.dart';
import 'package:flutter_web_bluetooth/flutter_web_bluetooth.dart';

import '../settings/settings_view.dart';
import 'device_detail.dart';

/// Displays a list of SampleItems.
class DeviceListView extends StatefulWidget {
  const DeviceListView({super.key});

  static const routeName = '/';
  @override
  State<DeviceListView> createState() => _DeviceListView();
}

class _DeviceListView extends State<DeviceListView> {
  StreamSubscription<Set<BluetoothDevice>>? _subscription;

  final _knownDevices = <BluetoothDevice>[];
  @override
  void initState() {
    super.initState();
    _subscription = FlutterWebBluetooth.instance.devices.listen((result) {
      for (final i in result) {
        if (!_knownDevices.any((r) => r.id == i.id)) {
          _knownDevices.add(i);
          setState(() {  }); // we could sort in here if we want...
        }
      }
    });
  }
  @override
  void dispose() {
    super.dispose();
    _subscription?.cancel();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('CLOK - Select/Add device'),
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
        itemCount: _knownDevices.length,
        itemBuilder: (BuildContext context, int index) => ListTile(
          title: Text('${_knownDevices[index].name}'),
          subtitle: Text(_knownDevices[index].id),
          onTap: () {
            // stop scan if scanning
            // if (_scanning) { _toggleScan(); }
            Navigator.pushNamed(context, DeviceDetailsView.routeName, arguments: _knownDevices[index].id);
        }),
      ),
      floatingActionButton: StreamBuilder(
        // Initialize FlutterFire:
        stream: FlutterWebBluetooth.instance.isAvailable,
        builder: (context, snapshot) {
          final requestOptions = RequestOptionsBuilder.acceptAllDevices(optionalServices: [
            BluetoothDefaultServiceUUIDS.deviceInformation.uuid
          ]);
          bool ready = (snapshot.data ?? false);
          return ready ? FloatingActionButton(
            onPressed: () {FlutterWebBluetooth.instance.requestDevice(requestOptions); }, // add new device
            tooltip: 'Add Clok',
            child: const Icon(Icons.search_off),
            ) : const SizedBox.shrink();
          }
      )
    );
  }
}
