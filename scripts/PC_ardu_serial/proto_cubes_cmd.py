# 
# Proto-CUBES Serial Port Commands
#  - write commands (ground to Proto-CUBES) are in capital letters
#  - read commands (ground from Proto-CUBES) are in non-capital letters
#
#       NB: Need to add 'b' in front of the command, to turn the definition into
#           a byte!
#
 
CMD_CITIROC_CONF    = b'C'
CMD_PROBE_CONF      = b'P'
CMD_HVPS_CONF       = b'H'

CMD_REQ_HK          = b'h'
CMD_REQ_PAYLOAD     = b'p'

