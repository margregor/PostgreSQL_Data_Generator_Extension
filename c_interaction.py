# myscript.py
from datetime import datetime
from personal_generator import PersonalGenerator
from adress_generator import AdressGenerator
import random

def interpret_type(type_str, person, adress):
    """Returns a placeholder value based on a type hint."""
    lower = type_str.strip().lower()

    if lower == "integer":
        return random.randint(1, 10000)
    elif lower == "float":
        return random.uniform(1.5, 9.5)
    elif lower == "bool":
        return random.choice([True, False])
    elif lower == "first_name":
        return person.first_name
    elif lower == "last_name":
        return person.last_name
    elif lower == "birth_date":
        return person.birth_date
    elif lower == "pesel":
        return person.pesel
    elif lower == "nip":
        return person.nip
    elif lower == "iban":
        return person.iban
    elif lower == "street":
        return adress.street
    elif lower == "house_number":
        return adress.house_number
    elif lower == "city":
        return adress.city
    elif lower == "postal_code":
        return adress.postal_code
    elif lower == "date":
        return datetime(random.randint(1900, 2025), random.randint(1,12), random.randint(1,28)).date()
    elif lower == "string":
        return "RANDOM_STRING"
    else:
        return f"Unknown type: {type_str}"

def generate_multiple_by_types(type_list, num_arrays):
    """Generates multiple arrays of objects based on type hints."""
    personal_generator = PersonalGenerator()
    adress_generator = AdressGenerator()
    result = []
    for _ in range(num_arrays):
        person = personal_generator.get_personal_data()
        adress = adress_generator.get_adress()
        # For each array, generate corresponding objects based on type_list
        result.append([interpret_type(t, person, adress) for t in type_list])
    return result
