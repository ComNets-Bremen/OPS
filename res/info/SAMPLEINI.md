# Details of the Sample .ini File

The `simulations` folder contains a sample `.ini` file with the name [`ops-omnetpp.ini`](../../simulations/ops-omnetpp.ini) that
contains a number of configurations (`Config` tags) to run OPS simulatioons with different
parameter configurations. The following table provides the details (brief) of each configuration.

|                `Config` tag                    |               Key Configuration Details                                                                 |
|                ------------                    |               -------------------------                                                                 |
| `[General]`                                    | Parameters required by all simulations (e.g., simulation time)                                          |
| `[Config Herald-Epidemic-SWIM]`                | Destination-less node model, Herald App, Epidemic forwarding, SWIM mobility                             |
| `[Config Herald-Keetchi-SWIM]`                 | Destination-less node model, Herald App, Keetchi forwarding, SWIM mobility                              |
| `[Config Herald-Optimum-Delay-SWIM]`           | Destination-less node model, Herald App, Optimum-delay forwarding, SWIM mobility                        |
| `[Config Herald-RRS-SWIM]`                     | Destination-less node model, Herald App, RRS forwarding, SWIM mobility                                  |
| `[Config Messenger-Epidemic-SWIM]`             | Destination oriented node model, Messenger App, Epidemic forwarding, SWIM mobility                      |
| `[Config Messenger-Epidemic-Single-Dest-SWIM]` | Destination oriented node model, Messenger App (single destination), Epidemic forwarding, SWIM mobility |
| `[Config Messenger-Keetchi-SWIM]`              | Destination oriented node model, Messenger App, Keetchi forwarding, SWIM mobility                       |
| `[Config Messenger-Optimum-Delay-SWIM]`        | Destination oriented node model, Messenger App, Optimum-delay forwarding, SWIM mobility                 |
| `[Config Messenger-PRoPHET-SWIM]`              | Destination oriented node model, Messenger App, PROPHET forwarding, SWIM mobility                       |
| `[Config Messenger-RRS-SWIM]`                  | Destination oriented node model, Messenger App, RRS forwarding, SWIM mobility                           |
| `[Config Messenger-Spray-n-Wait-SWIM]`         | Destination oriented node model, Messenger App, Spray & Wait forwarding, SWIM mobility                  |
| `[Config UBM-Epidemic-ExtendedSWIM]`           | Destination-less node model, UBM App, Epidemic forwarding, ExtendedSWIM mobility                        |
| `[Config UBM-Keetchi-ExtendedSWIM]`            | Destination-less node model, UBM App, Keetchi forwarding, ExtendedSWIM mobility                         |
