#include <onlmon/OnlMonServer.h>
#include <pmonitor/pmonitor.h>

R__LOAD_LIBRARY(libonlmonserver.so)
R__LOAD_LIBRARY(libonlmonserver_funcs.so)
void CleanUpServer();

void start_server(const char *prdffile = 0)
{
  OnlMonServer *se = OnlMonServer::instance(); // get pointer to Server Framework
  // set the ONLMONBBCLL1 Trigger definition (multiple triggers are possible)
//  se->OnlTrig()->AddBbcLL1TrigName("BBCLL1(>0 tubes) narrowvtx");
  if (!prdffile)
    {
      cout << "No Input file given" << endl;
      return;
    }
  if (!strcmp(prdffile, "etpool"))
    {
      // gSystem->Load("libcorbamsgbuffer.so");
      // corba_msg_buffer *enablecorbabuf = new corba_msg_buffer("monitor_event_channel");
//      petopen("/tmp/Monitor@etpool");
      prun();
      //  if etpool gets restarted we execute the EndRun and then
      // save the histograms
      se->EndRun(se->RunNumber()); //
      se->WriteHistoFile();
//      delete enablecorbabuf;
      CleanUpServer();
    }
  else if (!strcmp(prdffile, "et_test"))
    {
//      petopen("/tmp/Monitor@etpool");
    }
  else
    {
      //         gSystem->Load("libcorbamsgbuffer.so");
      //         corba_msg_buffer *enablecorbabuf = new corba_msg_buffer("monitor_event_channel");
      pfileopen(prdffile);
    }
  return;
}

void CleanUpServer()
{
  pclose();
  OnlMonServer *se = OnlMonServer::instance();
  delete se;
  gSystem->Exit(0);
}
