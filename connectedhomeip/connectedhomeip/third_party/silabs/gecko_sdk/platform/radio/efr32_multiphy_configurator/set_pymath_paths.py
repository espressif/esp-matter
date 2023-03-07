# Hack to handle distributing numpy, scypi, etc.. manually.
import os
import sys

platform_name = sys.platform
if platform_name == 'darwin' or platform_name.startswith('mac'):
    platform_dir_name = 'mac'
elif platform_name.startswith('linux'):
    platform_dir_name = 'linux'
elif platform_name.startswith('win'):
    platform_dir_name = 'win'
else:
    raise Exception('Unsupported OS platform: {}'.format(platform_name))

site_packages_path = os.path.abspath(os.path.join(os.path.dirname(__file__), 'studio_pymath/ext-site-packages/{}'.format(platform_dir_name)))
if not os.path.exists(site_packages_path):
    # If not in project's ./lib directory, then try and find in Studio's sdk path
    site_packages_path = os.path.abspath(os.path.join(site_packages_path[0:site_packages_path.find("sdk")], "adapter_packs/python/ext-site-packages"))

if not os.path.exists(site_packages_path):
    # If path not found, then try and find relative to jinja path
    import jinja2
    if platform_dir_name == "win":
        site_packages_path = os.path.abspath(os.path.join(os.path.dirname(jinja2.__file__), '../../../../../../../studio_pymath/ext-site-packages/{}'.format(platform_dir_name)))
    else:
        # mac and linux
        site_packages_path = os.path.abspath(os.path.join(os.path.dirname(jinja2.__file__), '../../../../../../../../studio_pymath/ext-site-packages/{}'.format(platform_dir_name)))

    if not os.path.exists(site_packages_path):
        if platform_dir_name == "win":
            site_packages_path = os.path.abspath(os.path.join(os.path.dirname(jinja2.__file__), '../../../../../studio_pymath/ext-site-packages/{}'.format(platform_dir_name)))
        else:
            # mac and linux
            site_packages_path = os.path.abspath(os.path.join(os.path.dirname(jinja2.__file__), '../../../../../../studio_pymath/ext-site-packages/{}'.format(platform_dir_name)))

    if not os.path.exists(site_packages_path):
        # If path not found, then try jinja path again...
        if platform_dir_name == "win":
            site_packages_path = os.path.abspath(os.path.join(os.path.dirname(jinja2.__file__), "../../../ext-site-packages"))
        else:
            site_packages_path = os.path.abspath(os.path.join(os.path.dirname(jinja2.__file__), "../../../../ext-site-packages"))

if os.path.exists(site_packages_path):
    if site_packages_path not in sys.path:
        sys.path.insert(0, site_packages_path)
else:
    print("ERROR: Unable to find (Numpy, Scipy) path (e.g. ext-site-packages)")
