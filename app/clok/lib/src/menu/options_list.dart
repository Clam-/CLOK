import 'package:flutter/material.dart';

import '../settings/settings_view.dart';
import 'simple_control.dart';
import 'device_detail.dart';

/// Displays a list of SampleItems.
class OptionsListView extends StatelessWidget {
  const OptionsListView({
    super.key,
    this.items = const [
      // WiFi
      //WiFiListControl("BAAD0011-5AAD-BAAD-FFFF-5AD5ADBADCLK", "WiFi SSIDs"),
      // RootCA
      SimpleControl("BAAD0031-5AAD-BAAD-FFFF-5AD5ADBADCLK", "RootCA URL", ""),
      // TZ
      SimpleControl("BAAD0051-5AAD-BAAD-FFFF-5AD5ADBADCLK", "TZ ZoneInfo URL", ""),
      //TZListControl("BAAD0051-5AAD-BAAD-FFFF-5AD5ADBADCLK", "TZ Select Timezone"),
      SimpleControl("BAAD0056-5AAD-BAAD-FFFF-5AD5ADBADCLK", "TZ NTP Server 1", ""),
      SimpleControl("BAAD0057-5AAD-BAAD-FFFF-5AD5ADBADCLK", "TZ NTP Server 2", ""),
    ],
  });

  static const routeName = '/';

  final List<SimpleControl> items;

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('CLOK Config'),
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
        restorationId: 'menuListView',
        itemCount: items.length,
        itemBuilder: (BuildContext context, int index) {
          return ListTile(
              title: const Text('SampleItem'),
              leading: const CircleAvatar(
                // Display the Flutter Logo image asset.
                foregroundImage: AssetImage('assets/images/flutter_logo.png'),
              ),
              onTap: () {
                // Navigate to the details page. If the user leaves and returns to
                // the app after it has been killed while running in the
                // background, the navigation stack is restored.
                Navigator.restorablePushNamed(
                  context,
                  DeviceDetailsView.routeName,
                );
              });
        },
      ),
    );
  }
}
