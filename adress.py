class Adress:
    def __init__(self):
        self._street = None
        self._street_prefix = None
        self._city = None
        self._postal_code = None
        self._house_number = None

    @property
    def street(self)->str:
        return self._street

    @street.setter
    def street(self, value):
        if not value:
            raise ValueError("Name cannot be empty")
        self._street = value
    
    @property
    def street_prefix(self)->str:
        return self._street_prefix

    @street_prefix.setter
    def street_prefix(self, value):
        if not value:
            raise ValueError("Name cannot be empty")
        self._street_prefix = value
    
    @property
    def city(self)->str:
        return self._city

    @city.setter
    def city(self, value):
        if not value:
            raise ValueError("Name cannot be empty")
        self._city = value
    
    @property
    def postal_code(self)->str:
        return self._postal_code

    @postal_code.setter
    def postal_code(self, value):
        if not value:
            raise ValueError("Name cannot be empty")
        self._postal_code = value

    @property
    def house_number(self)->str:
        return self._house_number

    @house_number.setter
    def house_number(self, value):
        if not value:
            raise ValueError("Name cannot be empty")
        self._house_number = value

    def to_string(self)->str:
        return f"{self.street_prefix} {self.street} {self.house_number} {self.postal_code} {self.city}"

    