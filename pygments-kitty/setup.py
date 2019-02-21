from setuptools import setup

setup(
    name='pygments-kitty',
    version='0.2',
    packages=[''],
    url='',
    license='MIT',
    author='Mads Grau Kristensen',
    author_email='madsgrau@gmail.com',
    description='',

    entry_points='''[pygments.lexers]
                    kitty=kitty_lexer:CustomLexer''',
)
