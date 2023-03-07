/******************************************************************************

 @file  erpc_setup_mesh.cpp

 @brief This file contains implementation of erpc mesh initiation

 Group: WCS BTS
 $Target Device: DEVICES $

 ******************************************************************************
 $License: BSD3 2016 $
 ******************************************************************************
 $Release Name: PACKAGE NAME $
 $Release Date: PACKAGE RELEASE DATE $
 *****************************************************************************/#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>

#include <erpc_client_setup.h>
#include <erpc_mbf_setup.h>
#include <erpc_server_setup.h>
#include <erpc_transport_setup.h>
#include <erpc_arbitrated_client_setup.h>
#include <erpc_manually_constructed.h>
#include <erpc_client_manager.h>
#include <erpc_arbitrated_client_manager.h>
#include <erpc_transport_arbitrator.h>
#include <erpc_simple_server.h>
#include <erpc_basic_codec.h>
#include <erpc_crc16.h>

#include <erpc_uart_simplelink_transport.h>
#include <ti/drivers/UART.h>

#define CONFIG_UART_PC              0

#include <lprf_api_server.h>

using namespace erpc;

/* IDL: lprf, server objects */
/* TODO - UartSimpleLinkTransport object */
static ManuallyConstructed<SimpleServer> lprf_server_api_ssobj;
static ManuallyConstructed<BasicCodecFactory> lprf_server_api_bcfobj;
static ManuallyConstructed<Crc16> lprf_server_api_crcobj;

/* IDL: lprf, callback client manager objects */
/* TODO - UartSimpleLinkTransport object */
static ManuallyConstructed<BasicCodecFactory> lprf_server_cbk_bcfobj;
static ManuallyConstructed<BasicCodec> lprf_server_cbk_bcobj;
static ManuallyConstructed<Crc16> lprf_server_cbk_crcobj;
static ManuallyConstructed<TransportArbitrator> lprf_server_taobj;
static ManuallyConstructed<ArbitratedClientManager> lprf_server_cbk_acmobj;

/* IDL: lprf, callback client manager interface declarations */
extern ClientManager *lprf_cbk_cm;

#if defined (__cplusplus)
extern "C" {
#endif

void erpc_lprf_server_setup(void)
{
    erpc_transport_t transport;
    erpc_mbf_t msg_buf_fact;
    /* TODO - UartSimpleLinkTransport object */
    erpc_transport_t arbitrator;

    /* create shared transport, arbitrator, and server objects */
    transport = erpc_transport_uart_simplelink_init(CONFIG_UART_PC, 115200);
    msg_buf_fact = erpc_mbf_static_init();
    arbitrator = erpc_arbitrated_client_init_ref(transport, msg_buf_fact,
            &lprf_server_cbk_acmobj, &lprf_server_taobj,
            &lprf_server_cbk_bcfobj, &lprf_server_cbk_bcobj, &lprf_server_cbk_crcobj);
    erpc_server_init_ref(arbitrator, msg_buf_fact,
            &lprf_server_api_ssobj, &lprf_server_api_bcfobj, &lprf_server_api_crcobj);

    /* add api services to rpc server */
    erpc_add_service_to_server_ref(create_BLEmesh_api_service(), &lprf_server_api_ssobj);

    /* bind callback interfaces to client manager */
    lprf_cbk_cm = lprf_server_cbk_acmobj;

    /* run the rpc server, never returns */
    erpc_server_run_ref(&lprf_server_api_ssobj);
}

/*
 *  ======== erpc_lprf_server_taskFxn ========
 */
void erpc_lprf_server_taskFxn(UArg arg0, UArg arg1)
{
    /*  Construct RPC objects
     *
     *  The erpc setup function must be called before any RPC
     *  calls are initiated.
     */
    erpc_lprf_server_setup();
}

void eRpcMesh_createTask()
{
  Task_Params task_p;

  /* create rpc server task */
  Task_Params_init(&task_p);
  task_p.priority = 4;
  task_p.stackSize = 0x800;
  Task_create(erpc_lprf_server_taskFxn, &task_p, NULL);
}

#if defined (__cplusplus)
}
#endif
