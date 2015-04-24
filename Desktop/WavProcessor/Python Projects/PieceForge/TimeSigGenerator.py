__author__ = 'mzapp'

import random
def generate_time_signature(sections=[], max_divisor=5, max_signatures=1, tries=100):
    for section in sections:
        try_count = 0
        while True:
            signatures = []

            for signature in range(max_signatures):
                beats = random.randrange(2, max_divisor, 1)
                signatures.append(beats)

            if (section.duration % sum(signatures)) == 0:
                new_section = section._replace(time_signature=signatures)
                sections[sections.index(section)] = new_section
                break

            try_count += 1

            if try_count > tries and max_signatures > 1:
                max_signatures -= 1
                try_count = 0


    return sections