from supsisim.RCPblk import RCPblk


def getTimerBlk(pout):
    """
    Call:   getTimerBlk(pout)

    Parameters
    ----------
       pout: connected output port(s)

    Returns
    -------
      Block's reprezentation RCPblk
    """

    return RCPblk("getTimer", [], pout, [0, 0], 0, [])
