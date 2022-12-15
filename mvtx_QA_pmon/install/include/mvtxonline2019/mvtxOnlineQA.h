#ifndef __MVTX_OM_H__
#define __MVTX_OM_H__

#include <pmonitor/pmonitor.h>   // Added by X. He following Martin's trick
#include <Event/Event.h>
#include <Event/EventTypes.h>
#include "TH2.h"

int process_event (Event *e); //++CINT
int process_histos(float thresh = 10);
int mask_pixels(float thresh = 5);
int analysis();
int OM();


int print_canvas();
int print_my_canvas();

int print_status();
void reset_histos();
void set_verbose(int v);
void set_refresh(int r);
void get_alignment();
void ReverseYAxis(TH2 * h);
void ConfirmXAxis(TH2 * h);
#endif /* __MVTX_OM_H__ */
