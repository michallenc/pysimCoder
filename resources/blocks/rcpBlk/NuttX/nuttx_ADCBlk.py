
from supsisim.RCPblk import RCPblk
from scipy import size

def nuttx_ADCBlk(pout, devname, n_ch, resolution, umin, umax):
    """

    Call:   nuttx_AdcBlk(pout, devname)

    Parameters
    ----------
       pout: connected output port(s)
       devname : Port

    Returns
    -------
       blk: RCPblk

    """
   
    if(size(pout) != n_ch):
       raise ValueError("Number of output (%i) should match number of channels (%i)" % (size(pin),n_ch))

    blk = RCPblk('nuttx_ADC', [], pout, [0,0], 0, [umin, umax], [n_ch, resolution, 0], devname)
    return blk
