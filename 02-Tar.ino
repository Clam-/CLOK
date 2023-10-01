void tarSetup() {
  
}

void unpackTZdata() {
  tarGzFS.begin();

  TarUnpacker *tar = new TarUnpacker();
  TARUnpacker->setTarVerify(true);                                                           // true = enables health checks but slows down the overall process
  TARUnpacker->setupFSCallbacks(targzTotalBytesFn, targzFreeBytesFn);                        // prevent the partition from exploding, recommended
  TARUnpacker->setLoggerCallback(BaseUnpacker::targzPrintLoggerCallback);                    // gz log verbosity
  TARUnpacker->setTarProgressCallback(BaseUnpacker::defaultProgressCallback);                // prints the untarring progress for each individual file
  TARUnpacker->setTarStatusProgressCallback(BaseUnpacker::defaultTarStatusProgressCallback); // print the filenames as they're expanded
  TARUnpacker->setTarMessageCallback(BaseUnpacker::targzPrintLoggerCallback);                // tar log verbosity

  Stream *streamptr = HTTP_CLIENT.getStreamPtr();
  if (streamptr != nullptr)
  {
    String contentLengthStr = HTTP_CLIENT.header("Content-Length");
    contentLengthStr.trim();
    int64_t streamSize = -1;
    if (contentLengthStr != "")
    {
      streamSize = atoi(contentLengthStr.c_str());
      Serial.printf("Stream size %d\n", streamSize);
    }

    if (!TARUnpacker->tarStreamExpander(streamptr, streamSize, tarGzFS, "/"))
    {
      Serial.printf("tarStreamExpander failed with return code #%d\n", TARUnpacker->tarGzGetError());
    }
    else
    {
      // print leftover bytes if any (probably zero-fill from the server)
      while (HTTP_CLIENT.connected())
      {
        size_t streamSize = streamptr->available();
        if (streamSize) { Serial.printf("%02x ", streamptr->read()); }
        else { break; }
      }

      Serial.println("Done");
      preferences.putString("tzver", )
    }
  }
  
}