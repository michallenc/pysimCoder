from supsisim.RCPblk import RCPblk


def TCPsocketAsyncBlk(*args):
    """
    Call:   TCPsocketAsyncBlk(*args)

    Parameters
    ----------
       pin: connected input port(s)
       pout: connected output port(s)
       params: block's parameters

    Returns
    -------
      Block's reprezentation RCPblk
    """

    if len(args) == 3:
        pin, pout, params = args
    elif len(args) == 2:
        pout = []
        pin, params = args

    return RCPblk("TCPsocketAsync", pin, pout, [0, 0], 1, params)
