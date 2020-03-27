Signature ::= ID Name Args
Name ::= '$' c-identifier "%"
Args ::= Arg | Arg Args | Eps | Fork
Arg ::= '%' Type '$' c-identifier '%'
Fork ::= "^" Arg '(' RemainingSigX ')'
RemainingSigX ::= RemainingSig | RemainingSig '|' RemainingSigX | Eps
RemainingSig ::= value ':' Signature

Type:
T: Bool
B: Byte
S: Short
I: Int
L: Long
F: Float
D: Double
V: VarInt
s: String
X: UUID
@[A-Za-z0-9@@]+: External Parser With Full Qualifier

Type Prefix:
!: Optional
U: Unsigned (integer)
[: Array Of
{: Array (Till Packet End)
