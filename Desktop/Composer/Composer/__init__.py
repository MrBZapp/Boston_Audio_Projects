__author__ = 'Matt'

from Composer import formbuilder
from music21 import corpus

def test():
    s = corpus.parse('bach/bwv65.2.xml')
    form = formbuilder.generate_form(5, 15)
    print(form)
    s.show()
