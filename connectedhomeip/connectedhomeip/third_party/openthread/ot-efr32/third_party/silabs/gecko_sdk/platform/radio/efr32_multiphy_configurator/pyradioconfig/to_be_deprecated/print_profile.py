from pycalcmodel.core.output import ModelOutputType


def return_category_list(model):

    category_list = []

    for input in model.profile.inputs:
        category = input.category
        if category not in category_list:
            category_list.append(category)
            
    for output in model.profile.get_outputs([]):
        category = output.category
        if category not in category_list:
            category_list.append(category)
        
    return category_list



def print_io_in_category(model, category):    

    print_lines = []
    for input in model.profile.inputs:
        name = input._readable_name
        desc = input._var._desc

        if input.category != category:
            continue
            
        print_lines.append( "Input:".ljust(10) + name.ljust(30) + desc)

    for output in model.profile.get_outputs([ModelOutputType.INFO]):
        name = output._readable_name
        desc = output._var._desc
        
        if output.category != category:
            continue
            
        print_lines.append("Output:".ljust(10) + name.ljust(30) + desc)
    
    return print_lines


#
#
#
def print_all_categories(model):


    for category in return_category_list(model):
        if category is '':
            print("\nNo category defined:")
        else:
            print("\n" + category + ":")

        
        for line in print_io_in_category(model, category):
            print("        " + line)
    
    
#
#
#
def print_profile(model):
    print_all_categories(model)
    
    