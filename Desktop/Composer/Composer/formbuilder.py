__author__ = 'Matt'

import random
def generate_form(sections, length):
    form = []

    for i in range(0, length):
        new_section = random.randrange(0, sections)
        form.append(new_section)

    # Get the unique order of the form
    seen = set()
    seen_add = seen.add
    unique_form = [x for x in form if not (x in seen or seen_add(x))]

    # Replace the indexes with ordered indexes
    return [unique_form.index(x) for x in form]