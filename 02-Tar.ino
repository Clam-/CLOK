
TarUnpacker *TAR_P;

void tarSetup() {
  tarGzFS.begin();
  TAR_P = new TarUnpacker();
}

void unpackTZdata(HTTPClient &hclient) {
  TAR_P->setTarVerify(true);                                                           // true = enables health checks but slows down the overall process
  TAR_P->setupFSCallbacks(targzTotalBytesFn, targzFreeBytesFn);                        // prevent the partition from exploding, recommended
  TAR_P->setLoggerCallback(BaseUnpacker::targzPrintLoggerCallback);                    // gz log verbosity
  TAR_P->setTarProgressCallback(BaseUnpacker::defaultProgressCallback);                // prints the untarring progress for each individual file
  TAR_P->setTarStatusProgressCallback(BaseUnpacker::defaultTarStatusProgressCallback); // print the filenames as they're expanded
  TAR_P->setTarMessageCallback(BaseUnpacker::targzPrintLoggerCallback);                // tar log verbosity

  Stream *streamptr = hclient.getStreamPtr();
  if (streamptr != nullptr)
  {
    String contentLengthStr = hclient.header("Content-Length");
    contentLengthStr.trim();
    int64_t streamSize = -1;
    if (contentLengthStr != "")
    {
      streamSize = atoi(contentLengthStr.c_str());
      Serial.printf("Stream size %d\n", streamSize);
    }

    if (!TAR_P->tarStreamExpander(streamptr, streamSize, tarGzFS, "/"))
    {
      Serial.printf("tarStreamExpander failed with return code #%d\n", TAR_P->tarGzGetError());
    }
    else
    {
      // print leftover bytes if any (probably zero-fill from the server)
      while (hclient.connected())
      {
        size_t streamSize = streamptr->available();
        if (streamSize) { Serial.printf("%02x ", streamptr->read()); }
        else { break; }
      }

      Serial.println("Done");
    }
  }
}