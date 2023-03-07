from pyradioconfig.parts.common.profiles.ocelot_regs import build_modem_regs_ocelot


def build_modem_regs_margay(model,profile):
    #Just add all of the Ocelot regs for now
    family = model.part_family.lower()
    build_modem_regs_ocelot(model, profile, family)
