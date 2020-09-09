# 1. T.O.C
- [1. T.O.C](#1-toc)
- [2. Tipos de mensagens](#2-tipos-de-mensagens)
  - [2.1. Todo ciclo](#21-todo-ciclo)
  - [2.2. Acknowledgebles](#22-acknowledgebles)
  - [2.3. Sporadicas](#23-sporadicas)
- [3. Detalhes da unidade de controle](#3-detalhes-da-unidade-de-controle)
  - [Em todos os modos](#em-todos-os-modos)
  - [3.1. Em modo de *inicialização*](#31-em-modo-de-inicialização)

# 2. Tipos de mensagens
## 2.1. Todo ciclo
Mensagens de informação que são trocadas todo ciclo.

From program:
- mode(n)

From units:
- pump_state(n, b)
- pump_control_state(n, b)
- level(v)
- steam(v)

## 2.2. Acknowledgebles
Mensagens mandadas toda transmissão até receber o devido acknowledgement.

From program:
- program_ready                     => physical_units_ready
- pump_failure_detection(n)         => pump_failure_acknowledgement(n)
- pump_control_failure_detection(n) => pump_control_failure_acknowledgement(n)
- level_failure_detection           => level_failure_acknowledgement
- steam_failure_detection           => steam_outcome_failure_acknowledgement

From units:
- pump_repaired(n)         => pump_repaired_acknowledgement(n)
- pump_control_repaired(n) => pump_control_repaired_acknowledgement(n)
- level_repaired           => level_repaired_acknowledgement
- steam_repaired           => steam_repaired_acknowledgement

## 2.3. Sporadicas
Mandadas somente em momentos específicos.

From program:
- valve: somente no modo *initialization*.
- open_pump(n)
- close_pump(n)

From units:
- stop: o programa deve ir pro modo *emergency_stop* quando recebida 3 vezes seguida.
- steam_boiler_waiting: somente no modo *initialization*. Inicia o programa de fato.

# 3. Detalhes da unidade de controle

A unidade de controle expôe a seguinte API para facilitar o manuseio de mensagens:

```cpp
using msg_from_units = boiler::messages::from_units::any;
using msg_to_units   = boiler::messages::to_units::any;

struct msg_handler {
    enum class response {keep_listening, unlisten};
    std::function<response(void)> on_missing;
    std::function<response(msg_from_units)> on_present;
};

template <typename Msg>
[[nodiscard]] auto expect() /* -> unspecified */;
template<typename Msg>
[[nodiscard]] auto send(Msg&&) /* -> unspecified */;

auto run_last(std::function<void(void)> func);
```

Sendo que `expect` retorna um objeto com as seguintes funcões:

```cpp
struct /* unspecified */ {
    auto eventually(std::function<void(msg_from_units)> on_receipt) && -> void;
    auto always(msg_handler) && -> void;
};
```

E `send`:

```cpp
struct /* unspecified */ {
    auto now() && -> void;
    auto until_ack(std::function<void(msg_from_units)> on_ack) && -> void;
};
```

Com isso em mente, o loop de transmissão e resposta de mensagens é o seguinte:

```cpp
auto boiler::control_unit::process_messages(
    std::vector<msg_from_units> messages)
    -> std::vector<msg_to_units>
{
    response.clear();

    handle_expected(messages);

    for(auto& deferred : run_last_handlers) {
        deferred();
    }
    run_last_handlers.clear();

    run_mode_routine();

    send(boiler::messages::to_units::mode{ mode_of_operation }).now();

    return response;
}
```

## Em todos os modos

Na maioria dos modos podemos administrar as mensagens entrando com o uso de
`expect<msg>().always(handler)`

## 3.1. Em modo de *inicialização*

> __§1.11__ — The *initialization mode* is the mode to start with. The program
> enters a state in which it waits for the message __STEAM_BOILER_WAITING__ to
> come from the physical units.

Pode ser solucionado com `expect<msg>().eventually(handler)` mas
como a vida não é tão simples:

> __§1.11__ — [...]. As soon as this message has been received the program checks
> whether the quantity of steam coming out of the steam-boiler is really zero.
> If the unit for detection of the level of steam is defective — that is, when
> v is not equal to zero — the program enters the *emergency stop* mode. If the
> quantity of water in the steam-boiler is above N2 the program activates the
> valve of the steam-boiler in order to empty it. If the quantity of water in
> the steam-boiler is below N1 then the program activates a pump to fill the
> steam-boiler. If the program realizes a failure of the water level detection
> unit it enters the *emergency stop* mode.

Idealmente essas verificações poderiam ser realizadas no `handler` porém, como
não há gariantia que as mensagens que carregam os dados chegaram antes de
`STEAM_BOILER_WAITING`, o `handler` deve fazer essas verificações com
`run_last(lambda)`.

> __§1.11__ — [...]. As soon as a level of water between N1 and N2 has been
> reached the program can send continuously the signal __PROGRAM_READY__ to
> the physical units until it receives the signal __PHYSICAL_UNITS_READY__
> which must necessarily be emitted by the physical units. As soon as this
> signal has been received, the program enters either the mode *normal* if
> all the physical units operate correctly or the mode *degraded* if any
> physical unit is defective. A transmission failure puts the program into
> the mode *emergency stop*.

Finalmente, podemos usar `send(msg).until_ack(lambda_on_ack)` aqui.
