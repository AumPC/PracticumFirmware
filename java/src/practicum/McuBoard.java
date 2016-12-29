package practicum;

import java.nio.*;
import java.util.*;
import org.usb4java.*;

public class McuBoard implements AutoCloseable
{
    private static final short VID = 0x16c0;
    private static final short PID = 0x05dc;
    private static final short TIMEOUT = 10;

    private static Context context;

    private DeviceHandle handle;
    private DeviceDescriptor descriptor;

    public McuBoard(Device device)
    {
        this.handle = new DeviceHandle();
        int result = LibUsb.open(device,handle);
        if (result != LibUsb.SUCCESS)
            throw new LibUsbException("Unable to open USB device", result);
        this.descriptor = new DeviceDescriptor();
        LibUsb.getDeviceDescriptor(device,descriptor);
    }

    public void close()
    {
        LibUsb.close(handle);
    }

    public static void initUsb()
    {
        context = new Context();
        int result = LibUsb.init(context);
        if (result != LibUsb.SUCCESS)
            throw new LibUsbException("Unable to initialize libusb.", result);
    }

    public static void cleanupUsb()
    {
        LibUsb.exit(context);
    }

    public int write(byte req, short index, short value)
    {
        ByteBuffer buffer = ByteBuffer.allocateDirect(8);
        
        int transfered = LibUsb.controlTransfer(handle, 
                (byte) (LibUsb.REQUEST_TYPE_VENDOR 
                    | LibUsb.RECIPIENT_DEVICE 
                    | LibUsb.ENDPOINT_OUT),
                (byte) req, (short) value, (short) index, buffer, TIMEOUT);
        if (transfered < 0)
            throw new LibUsbException("Control transfer failed", transfered);
        return transfered;
    }

    public byte[] read(byte req, short index, short value)
    {
        ByteBuffer buffer = ByteBuffer.allocateDirect(8);
        
        int transfered = LibUsb.controlTransfer(handle, 
                (byte) (LibUsb.REQUEST_TYPE_VENDOR 
                    | LibUsb.RECIPIENT_DEVICE 
                    | LibUsb.ENDPOINT_IN),
                (byte) req, (short) value, (short) index, buffer, TIMEOUT);
        if (transfered < 0)
            throw new LibUsbException("Control transfer failed", transfered);

        byte[] returned = new byte[transfered];
        buffer.get(returned);
        return returned;
    }

    public String getManufacturer()
    {
        return LibUsb.getStringDescriptor(
                handle,
                descriptor.iManufacturer());
    }

    public String getProduct()
    {
        return LibUsb.getStringDescriptor(
                handle,
                descriptor.iProduct());
    }

    public static Device[] findBoards()
    {
        /* Modified from an example at http://usb4java.org/quickstart/libusb.html */

        // Read the USB device list
        DeviceList list = new DeviceList();
        int result = LibUsb.getDeviceList(null, list);
        if (result < 0) throw new LibUsbException("Unable to get device list", result);

        LinkedList<Device> boards = new LinkedList<Device>();
        try
        {
            // Iterate over all devices and scan for practicum boards
            for (Device device: list)
            {
                DeviceDescriptor descriptor = new DeviceDescriptor();
                result = LibUsb.getDeviceDescriptor(device, descriptor);
                if (result != LibUsb.SUCCESS)
                    throw new LibUsbException("Unable to read device descriptor", result);
                if (descriptor.idVendor() == VID && descriptor.idProduct() == PID)
                    boards.add(device);
            }
        }
        finally
        {
            // Ensure the allocated device list is freed
            LibUsb.freeDeviceList(list, true);
        }

        return boards.toArray(new Device[0]);
    }
}


