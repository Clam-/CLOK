#define DEST_FS_USES_LITTLEFS
TAR_Unpacker *tar;
void tarSetup() {
  tarGzFS.begin();
  TAR_Unpacker = new TAR_Unpacker();
}

void unpackTZdata() {
  TAR_Unpacker->setTarVerify(true);                                                           // true = enables health checks but slows down the overall process
  TAR_Unpacker->setupFSCallbacks(targzTotalBytesFn, targzFreeBytesFn);                        // prevent the partition from exploding, recommended
  TAR_Unpacker->setLoggerCallback(BaseUnpacker::targzPrintLoggerCallback);                    // gz log verbosity
  TAR_Unpacker->setTarProgressCallback(BaseUnpacker::defaultProgressCallback);                // prints the untarring progress for each individual file
  TAR_Unpacker->setTarStatusProgressCallback(BaseUnpacker::defaultTarStatusProgressCallback); // print the filenames as they're expanded
  TAR_Unpacker->setTarMessageCallback(BaseUnpacker::targzPrintLoggerCallback);                // tar log verbosity

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

    if (!TAR_Unpacker->tarStreamExpander(streamptr, streamSize, tarGzFS, "/"))
    {
      Serial.printf("tarStreamExpander failed with return code #%d\n", TAR_Unpacker->tarGzGetError());
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
    }
  }
}