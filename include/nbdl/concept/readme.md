##  Concepts and associated functions in Nbdl:

### Producer
  - make_provider
  - send_upstream_message

### Consumer
  - make_consumer
  - send_downstream_message

### StateConsumer
  - make_state_consumer
  - notify_state_change

### Store
  - make_store
  - apply_action
  - has
  - get

### Message (not an actual concept at this point)
  - get_channel
  - get_action
  - get_path
  - get_maybe_is_from_root
  - get_maybe_uid
  - get_maybe_payload
  - get_maybe_private_payload
