class Source_Code(object):

    #
    # Create Python source code from a modem_model object.  Running the Python code
    # will initialize the model to the same state it was in memory.  This is used
    # to create templates for phy configuration source code.
    #
    @staticmethod
    def print_modem_model_forced_as_source(outputfilename, phy_name, modem_model):

        output_lines = list()

        line = 'def %s(modem_model): ' % phy_name
        #print(line)
        output_lines.append(line)

        for var in modem_model.vars:
            if var.svd_mapping:
                if var._value_forced is not None:       # gdc:  What's the right way to check this?
                    # MSW reformat enums correctly
                    if var.var_enum is not None:
                        # do something here to convert
                        # <WhitePolyEnum.NONE: 0>
                        # to
                        # model.vars.white_poly.var_enum.NONE
                        line = "    phy.profile_inputs.%s.value = model.vars.%s.var_enum.%s" % (
                        var.name, var.name, str(var.value).split(".")[1])
                        # pass
                    else:
                        line = "    phy.profile_inputs.%s.value = %r" % (var.name, var.value_forced)
                    #print(line)
                    output_lines.append(line)
            else:
                if var._value_forced is not None:
                    # MSW reformat enums correctly
                    if var.var_enum is not None:
                        # do something here to convert
                        # <WhitePolyEnum.NONE: 0>
                        # to
                        # model.vars.white_poly.var_enum.NONE
                        line = "    phy.profile_inputs.%s.value = model.vars.%s.var_enum.%s" % (
                        var.name, var.name, str(var.value).split(".")[1])
                    else:
                        line = "    phy.profile_inputs.%s.value = %r" % (var.name, var.value_forced)
                    #print(line)
                    output_lines.append(line)


        # write the lines to a file
        outputfile = open(outputfilename, 'w')
        for line in output_lines:
            outputfile.write('%s\n' % line)
        outputfile.write('\n')
        outputfile.close()