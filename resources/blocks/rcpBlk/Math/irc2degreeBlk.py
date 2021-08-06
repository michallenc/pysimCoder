from supsisim.RCPblk import RCPblk
from scipy import size

def irc2degreeBlk(pin, pout, offset):
    """

    Call:   clarke_forwardBlk(pin, pout)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)

    Returns
    -------
        blk  : RCPblk

    """

    blk = RCPblk('irc2degree', pin, pout, [0,0], 1, [], [offset])
    return blk
