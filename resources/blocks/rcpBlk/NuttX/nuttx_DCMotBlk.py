
from supsisim.RCPblk import RCPblk
from scipy import size

def nuttx_DCMotBlk(pin, port, freq):
    """

    Call:   nuttx_DCMotBlk(pin, pwm1, pwm2, do1, do2, ch, freq, umin, umax)

    Parameters
    ----------
       pin: connected input port(s)
       port : Port
       th : Threshold

    Returns
    -------
       blk: RCPblk

    """

    blk = RCPblk('nuttx_DCMot', pin, [], [0,0], 1, [freq], [], port)
    return blk
