import sys
import usb.core
import usb.util

print "Hi~"

# find our device
dev = usb.core.find(idVendor=0x16C0, idProduct=0x05DF)

# was it found?
if dev is None:
    raise ValueError('Device not found')

try:
    dev.detach_kernel_driver(0)
except: # this usually mean that kernel driver has already been dettached
    pass

# set the active configuration. With no arguments, the first
# configuration will be the active one
dev.set_configuration(1)

bytes = dev.read(0x81, 32)

print bytes

'''
# get an endpoint instance
cfg = dev.get_active_configuration()
intf = cfg[(0,0)]

ep = usb.util.find_descriptor(
    intf,
    # match the first OUT endpoint
    custom_match = \
    lambda e: \
        usb.util.endpoint_direction(e.bEndpointAddress) == \
        usb.util.ENDPOINT_OUT)

assert ep is not None

# write the data
ep.write('test')
'''
