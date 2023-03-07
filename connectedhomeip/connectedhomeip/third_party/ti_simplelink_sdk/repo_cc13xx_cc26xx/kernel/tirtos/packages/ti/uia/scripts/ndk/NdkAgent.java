package ti.uia.scripts.ndk;

import ti.uia.runtime.*;
import ti.uia.services.*;
import java.net.*;
import java.io.*;

import xdc.rta.MetaData;
import xdc.rov.*;

/*!
 *  ======== NdkAgent ========
 */
public class NdkAgent {

    /*! The IP address of the board, e.g. "146.252.161.22" */
    public String address;

    /*! The ports for sending and receiving. */
    public int msgUdpPort = 1234;
    public int eventUdpPort = 1235;

    /*! The number of cores. */
    public int numProcessors = 6;

    /*
     *  The sockets for sending and receiving data.
     */
    private Socket cmdSocket;
    private DatagramSocket eventSocket;
    private OutputStream outMsg;
    private InputStream inMsg;
    private RtaService service;

    /* Decoder for decoding response values from the target. */
    private TargetDecoder dec;

    /*
     *  ======== Constructor ========
     */
    public NdkAgent(MetaData meta)
    {
        /* Create a TargetEncoder based on the meta data */
        TargetEncoder enc = new TargetEncoder(meta.getEndianess(), meta.getBitsPerChar());

        service = new RtaService(enc, UIAPacket.getEndianess(meta.getEndianess()));

        dec = new TargetDecoder(meta.getEndianess(), meta.getBitsPerChar());
    }
    // TODO - Remove this hardcoded constructor.
    public NdkAgent()
    {
        /* Create a TargetEncoder, hardcoded for 6x targets. */
        TargetEncoder enc = new TargetEncoder(TargetType.Endianess.LITTLE, 8);

        service = new RtaService(enc, UIAPacket.Endianess.LITTLE);

        dec = new TargetDecoder(TargetType.Endianess.LITTLE, 8);
    }

    /*
     *  ======== openSockets ========
     */
    public void openSockets(Boolean supportControl) throws Exception
    {
        /* Convert the IP address into an InetAddress. */
        InetAddress inetAddr = InetAddress.getByName(this.address);

        if (supportControl == true) {
            cmdSocket = new Socket(inetAddr, this.msgUdpPort);

            outMsg = cmdSocket.getOutputStream();
            inMsg = cmdSocket.getInputStream();
        }

        eventSocket = new DatagramSocket(this.eventUdpPort);
        eventSocket.setReceiveBufferSize(65536);
    }

    /*
     *  ======== startAllCores =========
     *  Tells the RTA service on all cores to start pushing data to the host.
     */
    public void startAllCores() throws Exception
    {
        for (int destProc = 0; destProc < this.numProcessors; destProc++) {
            sendStartCmd(destProc);
        }
    }

    /*
     *  ======== setPeriodAllCores =========
     *  Tells the RTA service on all cores to start pushing data to the host.
     */
    public void setPeriodAllCores(int period) throws Exception
    {
        for (int destProc = 0; destProc < this.numProcessors; destProc++) {
            sendSetPeriodCmd(destProc, period);
        }
    }

    /*
     *  ======== sendStartCmd ========
     *  Creates a UIA msg to send to the specified core to start the RTA
     *  service on that core.
     */
    public void sendStartCmd(int destProc) throws Exception
    {
        /* Get the encoded start command */
        byte[] buf = service.getStartCmd(destProc);

        /* Send the command to the target. */
        System.out.println("Sending start command to core " + destProc);

        /* Send the command and receive acknowledgement. */
        sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_START_TX);
    }

    /*
     *  ======== sendStopCmd ========
     *  Creates a UIA msg to send to the specified core to stop the RTA
     *  service on that core.
     */
    public void sendStopCmd(int destProc) throws Exception
    {
        /* Get the encoded start command */
        byte[] buf = service.getStopCmd(destProc);

        /* Send the command to the target. */
        System.out.println("Sending stop command to core " + destProc);

        /* Send the command and receive acknowledgement. */
        sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_STOP_TX);
    }

    /*
     *  ======== sendSetPeriodCmd ========
     *  Creates a UIA msg to send to the specified core to set the period the RTA
     *  service on that core.
     */
    public void sendSetPeriodCmd(int destProc, int period) throws Exception
    {
        /* Get the encoded set period command */
        byte[] buf = service.getSetPeriodCmd(destProc, period);

        /* Send the command to the target. */
        System.out.println("Sending set period command to core " + destProc);

        /* Send the command and receive acknowledgement. */
        sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_CHANGE_PERIOD);
    }

    /*
     *  ======== sendLoggerEnableCmd ========
     *  Creates a UIA msg to send to the specified core to enable a logger
     */
    public void sendLoggerEnableCmd(int destProc, int loggerId) throws Exception
    {
        /* Get the encoded set period command */
        byte[] buf = service.getLoggerEnableCmd(destProc, loggerId);

        /* Send the command to the target. */
        System.out.println("Sending logger enable command to core " + destProc + " logger " + loggerId);

        /* Send the command and receive acknowledgement. */
        sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_LOGGER_ENABLE);
    }

    /*
     *  ======== sendLoggerDisableCmd ========
     *  Creates a UIA msg to send to the specified core to disable a logger
     */
    public void sendLoggerDisableCmd(int destProc, int loggerId) throws Exception
    {
        /* Get the encoded set period command */
        byte[] buf = service.getLoggerDisableCmd(destProc, loggerId);

        /* Send the command to the target. */
        System.out.println("Sending logger disable command to core " + destProc + " logger " + loggerId);

        /* Send the command and receive acknowledgement. */
        sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_LOGGER_DISABLE);
    }

    /*
     *  ======== sendLoggerResetCmd ========
     *  Creates a UIA msg to send to the specified core to reset a logger
     */
    public void sendLoggerResetCmd(int destProc, int loggerId) throws Exception
    {
        /* Get the encoded set period command */
        byte[] buf = service.getLoggerResetCmd(destProc, loggerId);

        /* Send the command to the target. */
        System.out.println("Sending logger reset command to core " + destProc + " logger " + loggerId);

        /* Send the command and receive acknowledgement. */
        sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_LOGGER_RESET);
    }

    /*
     *  ======== sendLoggerEnableCmd ========
     *  Creates a UIA msg to send to the specified core to enable all loggers
     */
    public void sendLoggerEnableAllCmd(int destProc) throws Exception
    {
        /* Get the encoded set period command */
        byte[] buf = service.getLoggerEnableAllCmd(destProc);

        /* Send the command to the target. */
        System.out.println("Sending logger enable all command to core " + destProc);

        /* Send the command and receive acknowledgement. */
        sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_LOGGER_ENABLE_ALL);
    }

    /*
     *  ======== sendLoggerDisableCmd ========
     *  Creates a UIA msg to send to the specified core to disable all loggers
     */
    public void sendLoggerDisableAllCmd(int destProc) throws Exception
    {
        /* Get the encoded set period command */
        byte[] buf = service.getLoggerDisableAllCmd(destProc);

        /* Send the command to the target. */
        System.out.println("Sending logger disable all command to core " + destProc);

        /* Send the command and receive acknowledgement. */
        sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_LOGGER_DISABLE_ALL);
    }

    /*
     *  ======== sendLoggerResetAllCmd ========
     *  Creates a UIA msg to send to the specified core to reset all loggers
     */
    public void sendLoggerResetAllCmd(int destProc) throws Exception
    {
        /* Get the encoded set period command */
        byte[] buf = service.getLoggerResetAllCmd(destProc);

        /* Send the command to the target. */
        System.out.println("Sending logger reset all command to core " + destProc);

        /* Send the command and receive acknowledgement. */
        sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_LOGGER_RESET_ALL);
    }

    /*
     *  ======== sendWriteMaskCmd ========
     *  Creates a UIA msg to send to the specified core to write a Diags
     *  mask.
     */
    public void sendWriteMaskCmd(int destProc, long addr, long mask) throws Exception
    {
        /* Get the encoded set period command */
        byte[] buf = service.getWriteMaskCmd(destProc, addr, mask);

        /* Send the command to the target. */
        System.out.println("Sending write mask command to core " + destProc + " addr = " + Long.toHexString(addr) + " mask = " + Long.toHexString(mask));

        /* Send the command and receive acknowledgement. */
        sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_WRITE_MASK);
    }

    /*
     *  ======== sendSnapshotCmd ========
     *  Creates a UIA msg to send to the specified core to snapshot request.
     */
    public void sendSnapshotCmd(int destProc, long resetFlag, long periodInMs) throws Exception
    {
        /* Get the encoded set period command */
        byte[] buf = service.getLoggerSnapshotAll(destProc, resetFlag, periodInMs);

        /* Send the command to the target. */
        System.out.println("Sending snapshot command to core " + destProc + " resetFlag = " + resetFlag + " periodInMs = " + periodInMs);

        /* Send the command and receive acknowledgement. */
        sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_SNAPSHOT_ALL);
    }

    /*
     *  ======== sendReadMaskCmd ========
     *  Creates a UIA msg to send to the specified core to read a Diags
     *  mask.
     */
    public void sendReadMaskCmd(int destProc, long addr) throws Exception
    {
        /* Get the encoded set period command */
        byte[] buf = service.getReadMaskCmd(destProc, addr);

        /* Send the command to the target. */
        System.out.println("Sending read mask command to core " + destProc + " addr = 0x" + Long.toHexString(addr));

        /* Send the command and receive acknowledgement. */
        byte[] resp = sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_READ_MASK);

        //TODO look at payload endianness bit in the header.
        long mask;

        /* Read the mask value from the response payload. */
        mask = dec.decodeBytes(resp, MessagePacketHeader.getHeaderSize(), 4, false);

        //if (false) {
            /* Big Endian payload */
            //mask = (resp[16] << 24) | (resp[17] << 16) | (resp[18] << 8) | resp[19];
        //}
        //else {
            /* Little Endian payload */
        //    mask = (resp[16]) | (resp[17] << 8) | (resp[18] << 16) | (resp[19] << 24);
        //}

        System.out.println("diagsMask__C = 0x" + Long.toHexString(mask));
    }

    /*
     *  ======== sendGetCpuFreqCmd ========
     *  Creates a UIA msg to send to the specified core to get the CPU frequency.
     */
    public void sendGetCpuFreqCmd(int destProc) throws Exception
    {
        /* Get the encoded set period command */
        byte[] buf = service.getCpuFreqCmd(destProc);

        /* Send the command to the target. */
        System.out.println("Sending get CPU frequency to core " + destProc);

        /* Send the command and receive acknowledgement. */
        byte[] resp = sendCmd(buf, MessagePacketHeader.EnumMsgType.ACK, RtaService.CMD_GET_CPU_SPEED);

        /*
         * Decode the response arguments. These are the hi and lo components
         * of the frequency.
         */
        long hi = dec.decodeBytes(resp, MessagePacketHeader.getHeaderSize(), 4, false);
        long lo = dec.decodeBytes(resp, MessagePacketHeader.getHeaderSize() + 4, 4, false);

        /*
         * Combine these to get the full 64-bit value.
         * Note - Java cannot do a 32-bit shift, you must multiply by 2^32.
         */
        long freq = (long) (hi * Math.pow(2, 32)) + lo;

        /*
        //TODO look at payload endianness bit in the header.
        long total = 0;
        if (false) {
            // Big Endian payload
            for (int i = 16; i < 24; i++)
            {
                total = (total << 8) + (resp[i] & 0xff);
            }
        }
        else {
            // Little Endian payload
            for (int i = 16; i < 24; i++)
            {
                total += (resp[i] & 0xff) << (8 * i);
            }
        }
        */
        System.out.println("CPU Frequency = " + Long.toString(freq));
    }

    /*
     *  ======== sendCmd ========
     */
    public byte[] sendCmd(byte[] buffer, MessagePacketHeader.EnumMsgType msgType, int cmdId) throws Exception
    {
        /* Send the command down. */
        outMsg.write(buffer, 0, buffer.length);
        System.out.println("sent message cmdId - " + cmdId);
        /* Wait until we receive acknowledgement of the command. */
        while (true) {

            /* Wait for a response */
            byte[] respBuf = readResponse();

            // TODO - First check the header type.

            MessagePacketHeader header = new MessagePacketHeader();

            /* Decode the message packet header. */
            header.read(respBuf, 0);

            /*
             *  - Tag should match
             *  - MsgType should be ACK (0)
             *  - cmdId should match the cmd, 7 - start
             *  TODO - Should seqNum match?
             *  TODO - How much should we validate? Maybe tag is enough?
             *  TODO - Check whether packet is actually msg
             */
            //if ((header.getMsgType() == msgType) &&
            //    (header.getCmdId() == cmdId)) {
           if (header.getCmdId() == cmdId) {
                System.out.println("Received cmd ACK");
                return (respBuf);
            }
            else {
                // TODO
                System.out.println("Received message " + header.getMsgType() + " " + header.getCmdId() );
                System.out.println("Received something other than an ACK for the response...");
            }
        }
    }

    /*
     *  ======== readResponse ========
     */
    public byte[] readResponse() throws Exception
    {
        byte[] rxBuf = new byte[1500];

        int readLen = inMsg.read(rxBuf, 0, rxBuf.length);

        // TODO - This probably isn't necessary.
        byte[] retBuf = new byte[readLen];
        System.arraycopy(rxBuf, 0, retBuf, 0, readLen);

        return (retBuf);
    }

    /*
     *  ======== readEvents ========
     */
    public DatagramPacket readEvents() throws Exception
    {
        byte[] rxBuf = new byte[1500];

        DatagramPacket packet = new DatagramPacket(rxBuf, rxBuf.length);

        eventSocket.receive(packet);

        return (packet);
    }

}
