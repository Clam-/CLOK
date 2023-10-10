import 'package:flutter/material.dart';

/// A placeholder class that represents an entity or model.
class SimpleControl {
  const SimpleControl(this.characteristic, this.optionName, this.optionValue);
  final String characteristic;
  final String optionName;
  final String optionValue;

  void Function() onTapGen(BuildContext context) {
    // dialog here...
    
    return () {
      showDialog<String>(
        context: context,
        builder: (BuildContext context) {
          TextEditingController textFieldController = TextEditingController();
          return AlertDialog(
            title: Text(optionName),
            content: TextField(
              controller: textFieldController,
              decoration: const InputDecoration(hintText: "Value"),
            ),
            actions: <Widget>[
              TextButton (
                child: const Text('Cancel'),
                onPressed: () {
                  Navigator.pop(context);
                },
              ),
              FilledButton (
                child: const Text('Save'),
                onPressed: () {
                  print(textFieldController.text);
                  // send value to characteristic...
                  Navigator.pop(context);
                },
              ),
            ],
          );
        }
      );
    };
  }
}
