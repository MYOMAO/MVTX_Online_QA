#include <CommonFuncs.C>

#include <onlmon/mvtx/MvtxMonDraw.h>

#include <onlmon/OnlMonClient.h>

R__LOAD_LIBRARY(libonlmvtxmon_client.so)

void mvtxDrawInit(const int online = 0)
{
  OnlMonClient *cl = OnlMonClient::instance();
  // register histos we want with monitor name
  cl->registerHisto("mvtxmon_hist1", "MVTXMON");
  cl->registerHisto("mvtxmon_hist2", "MVTXMON");
  cl->AddServerHost("localhost");   // check local host first
  CreateHostList(online);
  // get my histos from server, the second parameter = 1
  // says I know they are all on the same node
  cl->requestHistoBySubSystem("MVTXMON", 1);
  OnlMonDraw *mvtxmon = new MvtxMonDraw();    // create Drawing Object
  cl->registerDrawer(mvtxmon);              // register with client framework
}

void mvtxDraw(const char *what="ALL")
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->requestHistoBySubSystem("MVTXMON");         // update histos
  cl->Draw("MVTXMON",what);                       // Draw Histos of registered Drawers
}

void mvtxPS()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakePS("MVTXMON");                          // Create PS files
  return;
}

void mvtxHtml()
{
  OnlMonClient *cl = OnlMonClient::instance();  // get pointer to framewrk
  cl->MakeHtml("MVTXMON");                        // Create html output
  return;
}
