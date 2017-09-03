# Log Encodings

The logs that are created by OPS contain activity information which are encoded
using the following codes. When developing new models, please reuse the codes
if the information that the new model attempts to log is the same or similar. 

|  Code    |        Description         |
|  ---     |             ---            |
| **Application Layer Codes**  |        |
| GF       | Generated Feedback         | 
| T-Imm    | Type - Immediate           |
| T-Pre    | Type - Preference          |
| NLB      | Notification List Begin    |
| C        | Count                      |
| NE       | Notification Entry         |
| L        | LOVE                       |
| I        | IGNORE                     |
| NLE      | Notification List End      |
| SUTG     | Setting up to generate     |
| DI       | Data Items                 |
| GAR      | Generated App Registration |
| GD       | Generated Data             |
| RD       | Received Data              |
| AS       | At Source                  |
| AD       | At Destination             |
| **Forwarding Layer Codes**   |        |
| UI       | Upper In                   |
| DM       | Data Msg                   |
| FM       | Feedback Msg               |
| NM       | Neighbour Msg              |
| NC       | Neighbour Count            |
| CS       | Cache Size                 |
| LO       | Lower Out                  |
| SVM      | Summary Vector Msg         |
| CE       | Cache Entries              |
| LI       | Lower In                   |
| UO       | Upper Out                  |
| DRM      | Data Request Msg           |
| **Link Layer Codes**          |       |
| NI       | Neighbourhood Info         |

