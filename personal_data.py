class Person:
    def __init__(self, first_name, last_name, birth_date, pesel, nip, iban):
        self._first_name = first_name
        self._last_name = last_name
        self._birth_date = birth_date
        self._pesel = pesel
        self._nip = nip
        self._iban = iban

    @property
    def first_name(self):
        return self._first_name

    @first_name.setter
    def first_name(self, value):
        self._first_name = value

    @property
    def last_name(self):
        return self._last_name

    @last_name.setter
    def last_name(self, value):
        self._last_name = value

    @property
    def birth_date(self):
        return self._birth_date

    @birth_date.setter
    def birth_date(self, value):
        self._birth_date = value

    @property
    def pesel(self):
        return self._pesel

    @pesel.setter
    def pesel(self, value):
        self._pesel = value

    @property
    def nip(self):
        return self._nip

    @nip.setter
    def nip(self, value):
        self._nip = value
    
    @property
    def iban(self):
        return self._iban

    @iban.setter
    def iban(self, value):
        self._iban = value

    def to_string(self):
        return f"Imię: {self.first_name}, Nazwisko: {self.last_name}, Data urodzenia: {self.birth_date}, PESEL: {self.pesel}, NIP: {self.nip}, IBAN: {self.iban}"
