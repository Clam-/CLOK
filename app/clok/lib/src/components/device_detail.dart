import 'dart:async';

import 'package:clok/src/components/control_base.dart';
import 'package:clok/src/components/control_wifideleter.dart';
import 'package:flutter_web_bluetooth/flutter_web_bluetooth.dart';
import 'package:flutter/material.dart';

import '../consts.dart';
import '../settings/settings_view.dart';
import 'control_status.dart';
import 'control_string.dart';
import 'control_toggle.dart';
import 'control_tzselect.dart';
import 'control_wifipicker.dart';

/// Displays detailed information about a SampleItem.
class DeviceDetailsView extends StatefulWidget {
  const DeviceDetailsView({super.key, required this.device});
  static const routeName = '/device';
  final BluetoothDevice device;

@override
  State<DeviceDetailsView> createState() => _DeviceDetailsView();
}

class _DeviceDetailsView extends State<DeviceDetailsView> {

  // state vars
  bool _bleConnected = false;
  StreamSubscription? connectListener;
  final items = <BaseControl>[] = []; // Options List
  BaseControl? wifiScanControl;
  @override
  void initState() {
    super.initState();
    // setup handlers
    if (widget.device.hasGATT) {
      connectListener = widget.device.connected.listen(_handleConnectionChange);
      widget.device.connect().catchError(_onConnectError);
    }
  }
  FutureOr<void> _onConnectError(Object o) {
    print("Connect error: $o");
    // attempt reconnect
    widget.device.connect().catchError(_onConnectError);
  }
  void _handleConnectionChange(bool connected) {
    print('_handleConnectionChange $connected');
    //if (!mounted) { return; }
    if (connected) { _onConnect(); setState(() { _bleConnected = true; });  }
    else { 
      // ignore initial connected state...
      if (_bleConnected != false) { Navigator.pop(context); }
    } // disconencted
  }
  
  void _onConnect() {
    // discover services
    if (_bleConnected) { return; } // bail if already connected so we don't double/triple request.
    widget.device.discoverServices().then(_handleServiceDiscovery);
    print("...Discovering...");
  }

  void _handleServiceDiscovery(List<BluetoothService> services) async {
    print('_handleServiceDiscovery $services');
    if (!mounted) { return; }
    for (final service in services) {
      print("Processing ${service.uuid}");
      if (service.uuid == SERVICE_ID) {
        if (items.isNotEmpty) { continue; } // skip if have already added items
        // store reference to extra characteristics
        final Map<String, BluetoothCharacteristic> extraBCs = {};
        for (final chara in await service.getCharacteristics()) {
          BaseControl? control;
          // ugly.
          switch(chara.uuid) {
            case ROOTCA_URL:
              if (!items.contains("RootCA URL")) { // guards justtttt in case we were somehow still connected to a device.
                control = StringControl(setState, chara, "RootCA URL", "");
                items.add(control);
              } else { print("WE DOUBLINGGGGGGGGGG."); }
            case TZ_ZONEINFO_URL:
              if (!items.contains("TZ ZoneInfo URL")) { 
                control = StringControl(setState, chara, "TZ ZoneInfo URL", "");
                items.add(control);
              }
            case TZ_NTP1:
              if (!items.contains("TZ NTP Server 1")) { 
                control = StringControl(setState, chara, "TZ NTP Server 1", "");
                items.add(control);
              }
            case TZ_NTP2:
              if (!items.contains("TZ NTP Server 2")) { 
                control = StringControl(setState, chara, "TZ NTP Server 2", "");
                items.add(control);
              }
            case WIFI_SSIDS:
              if (!items.contains("Add WiFi SSID & Key")) { 
                control = WiFiPickerControl(setState, chara, "Add WiFi SSID & Key", "", extrachars: extraBCs);
                items.add(control); 
                extraBCs.clear();
              }
            case WIFI_REQUESTSSID:
              extraBCs["wsc"] = chara; // assuming BC broadcast order is consistent
            case WIFI_WPAKEY:
              extraBCs["wkc"] = chara; // ditto
            case WIFI_DOSCAN:
              if (!items.contains("WiFi Scan")) { 
                wifiScanControl = ToggleControl(setState, chara, "WiFi Scan", false, display: false); 
              }
            case WIFI_KNOWN:
              if (!items.contains("Known SSIDs")) { 
                control = WiFiDeleteControl(setState, chara, "Known SSIDs", "", extrachars: extraBCs);
                items.add(control);
                extraBCs.clear();
              }
            case WIFI_DELETE:
              // assume this gets processed before WIFI_KNOWN... hopefully this doesn't break in the future...
              extraBCs["wdc"] = chara;
            case TZ_REGIONS:
              if (!items.contains("Timezone location")) { 
                control = TZSelectControl(setState, chara, "Timezone location", "", extrachars: extraBCs);
                items.add(control);
                extraBCs.clear();
              }
            case TZ_REGION:
              extraBCs["rc"] = chara; // assuming BC broadcast order is consistent
            case TZ_TIMEZONES:
              extraBCs["tsc"] = chara; // ditto
            case TZ_TIMEZONE:
              extraBCs["tzc"] = chara; // ditto
            case WIFI_STATUS:
              if (!items.contains("WiFi Status")) { 
                control = StatusControl(setState, chara, "WiFi Status", -3);
                items.add(control);
              }
          }
          if (chara.properties.notify && !chara.isNotifying) {
            print("setup notifier ${chara.uuid}");
            await chara.startNotifications().catchError((FutureOr<void> e) {print("Why? $e - ${chara.uuid} - ${chara.properties.hasNotify}"); });
          }
          // init control if needed
          await control?.init();
          control = null;
          setState(() {  }); // does this update the view? the answer is yes.
        }
      }
    }
  }
  @override
  void dispose() {
    var device = widget.device; // so we can free up the widget reference and not refer back to it after disposed.
    Timer(const Duration(milliseconds: 1), () async {
      await connectListener?.cancel();
      device.disconnect();
    });
    print("...DISPOSE...");
    super.dispose();
  }


  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('CLOK - Settings'),
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
      body:
      Column(
        children: <Widget>[
          Text("Device: ${widget.device.name} (${widget.device.id})"),
          Expanded(
            child: ListView.builder(
              // Providing a restorationId allows the ListView to restore the
              // scroll position when a user leaves and returns to the app after it
              // has been killed while running in the background.
              restorationId: 'menuListView',
              itemCount: items.length,
              itemBuilder: (BuildContext context, int index) {
                return ListTile(
                    enabled: items[index].display,
                    title: Text(items[index].optionName),
                    subtitle: Text(items[index].toString()),
                    onTap: items[index].onTapGen(context)
                  );
              },
            ),
          )
        ],
      ),
      floatingActionButton: FloatingActionButton(
        onPressed: _bleConnected ? () { wifiScanControl?.sendData(!wifiScanControl!.optionValue); } : (){},
        tooltip: _bleConnected ? wifiScanControl != null && wifiScanControl?.optionValue ? 'Stop Scan' : 'Start Scan' : 'Not ready to scan for WiFi networks',
        child: _bleConnected ? wifiScanControl != null && wifiScanControl?.optionValue ? const Icon(Icons.signal_wifi_statusbar_null) : const Icon(Icons.wifi_find) : const Icon(Icons.signal_wifi_bad),
      )
    );
  }
}
