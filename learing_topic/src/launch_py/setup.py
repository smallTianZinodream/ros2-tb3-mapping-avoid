import os
from glob import glob

from setuptools import find_packages, setup

package_name = 'launch_py'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch', 'py'),
            glob(os.path.join('launch', 'py', '*.py'))),
        (os.path.join('share', package_name, 'launch', 'xml'),
            glob(os.path.join('launch', 'xml', '*.xml'))),
        (os.path.join('share', package_name, 'launch', 'yaml'),
            glob(os.path.join('launch', 'yaml', '*.yaml'))),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='jsetc',
    maintainer_email='jsetc@todo.todo',
    description='Launch files demo for turtlesim',
    license='Apache-2.0',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
        ],
    },
)
