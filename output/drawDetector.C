void drawDetector()
{
  TGeoManager::Import("sipmwheel.gdml");

  gGeoManager->SetNsegments(100);
//  gGeoManager->SetVisLevel(1);
  gGeoManager->GetTopVolume()->Raytrace();
  //gGeoManager->GetTopVolume()->Draw();
}
