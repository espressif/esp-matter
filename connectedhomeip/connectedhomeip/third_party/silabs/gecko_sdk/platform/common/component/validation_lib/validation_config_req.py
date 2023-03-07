def validate_config_component_req(project, config, value, component_id):
  config_obj = project.config(config)

  if config_obj.value() == value and project.is_selected(component_id) == 0:
    comp = project.component(component_id)
    comp_name = comp.label()

    project.error('Component ' + comp_name + ' must be selected when ' + config + ' is set to ' + value + '.',
      config_obj.file_name(),
      '')


def validate_boolean_config_req(project, config, config_needed):
  config_obj = project.config(config)
  config_needed_obj = project.config(config_needed)

  if config_obj.value() == '1' and config_needed_obj.value() == '0':
    project.error('Configuration ' + config_needed_obj.id() + ' must be selected when ' + config_obj.id() + ' is selected in component ' + config_obj.component().label() + '.',
      config_obj.file_name(),
      '')
