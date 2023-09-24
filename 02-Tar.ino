void unpackTZdata() {
  tarGzFS.begin();

  TarUnpacker *tar = new TarUnpacker();
  tar->haltOnError(false);
  tar->setTarVerify( true );
  tar->setupFSCallbacks( targzTotalBytesFn, targzFreeBytesFn );
  tar->setLoggerCallback(BaseUnpacker::targzPrintLoggerCallback);
  tar->setTarProgressCallback( BaseUnpacker::defaultProgressCallback );
  tar->setTarStatusProgressCallback( BaseUnpacker::defaultTarStatusProgressCallback ); // print the filenames as they're expanded
  tar->setTarMessageCallback( BaseUnpacker::targzPrintLoggerCallback ); // tar log verbosity

  if (!tar->tarStreamExpander(streamptr, streamSize, tarGzFS, "/static"))
  {
    Serial.printf("tarStreamExpander failed with return code #%d\n", tar->tarGzGetError());
  }
  preferences.putString("tzver", )
  
}