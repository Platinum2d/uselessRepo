from django.db.models import Model, IntegerField, CharField, DateField, BooleanField, ForeignKey, OneToOneField, DO_NOTHING, CASCADE
from django.urls import reverse

club_name_max_length = 20
person_name_max_length = 40
surname_max_length = 50
group_name_length = 1
nation_max_length = 30
stadium_max_length = 40


class Referee(Model):
    name = CharField(max_length=person_name_max_length)
    surname = CharField(max_length=surname_max_length)

    def __str__(self):
        return self.name.__str__() + ' ' + self.surname.__str__()


class Coach(Model):
    name = CharField(max_length=person_name_max_length)
    surname = CharField(max_length=surname_max_length)

    class Meta:
        verbose_name = "Coach"
        verbose_name_plural = "Coaches"

    def __str__(self):
        return self.name.__str__() + ' ' + self.surname.__str__()


class Group(Model):
    letter = CharField(primary_key=True, max_length=group_name_length)

    def __str__(self):
        return '' + self.letter.__str__()


class Club(Model):
    name = CharField(primary_key=True, max_length=club_name_max_length)
    coach = OneToOneField(to=Coach, related_name="coach", on_delete=CASCADE)
    group = ForeignKey(to=Group, on_delete=DO_NOTHING, related_name="clubs")
    stadium = CharField(max_length=stadium_max_length, default="None")

    def get_absolute_url(self):
        return reverse("club_details", kwargs={"pk": self.pk})

    def __str__(self):
        return '' + self.name.__str__()


class Footballer(Model):
    name = CharField(max_length=person_name_max_length)
    surname = CharField(max_length=surname_max_length)
    number = IntegerField(null=False)
    date_of_birth = DateField()
    nation = CharField(max_length=nation_max_length)
    club = ForeignKey(null=False, to=Club, on_delete=CASCADE, related_name="footballers")

    def get_absolute_url(self):
        return reverse("footballer_details", kwargs={"pk": self.pk})

    def __str__(self):
        return self.surname.__str__() + " " + self.name.__str__()


class Match(Model):
    referee = ForeignKey(to=Referee, on_delete=DO_NOTHING, related_name="matches")
    home_club = ForeignKey(to=Club, on_delete=CASCADE, related_name="home_matches")
    away_club = ForeignKey(to=Club, on_delete=CASCADE, related_name="away_matches")
    date = DateField(default="1970-01-01")

    class Meta:
        verbose_name = "Match"
        verbose_name_plural = "Matches"

    def get_absolute_url(self):
        return reverse("match_details", kwargs={"pk": self.pk})

    def __str__(self):
        return self.home_club.__str__() + ' - ' + self.away_club.__str__()


class Presence(Model):
    footballer = ForeignKey(to=Footballer, on_delete=CASCADE, related_name="presences")
    match = ForeignKey(to=Match, on_delete=CASCADE, related_name="presences")
    is_in_starting_XI = BooleanField(default=True)

    def __str__(self):
        string = self.match.__str__() + ' - ' + self.footballer.__str__() + " - "
        if self.is_in_starting_XI:
            string += "Starting XI"
        else:
            string += "Bench"

        return string


class Substitution(Model):
    match = ForeignKey(to=Match, on_delete=CASCADE, related_name="substitutions")
    replaced = ForeignKey(to=Footballer, on_delete=CASCADE, related_name="substitutions")
    replacement = ForeignKey(to=Footballer, on_delete=CASCADE, related_name="replacements")
    minute = IntegerField(default=0)

    def __str__(self):
        return self.match.__str__() + " - " + self.replacement.__str__() + ' for ' + self.replaced.__str__()


class GroupMatch(Model):
    group = ForeignKey(null=False, to=Group, on_delete=CASCADE, related_name="matches")
    referred_match = ForeignKey(null=False, to=Match, on_delete=CASCADE, related_name="is_group_match")
    day = IntegerField(default=0)

    class Meta:
        unique_together = (('group', 'referred_match'),)
        verbose_name = "Group match"
        verbose_name_plural = "Group matches"

    def __str__(self):
        return 'Group ' + self.group.__str__() + ' day ' + self.day.__str__() + ' : ' + self.referred_match.__str__()


class KnockoutMatch(Model):
    turn = IntegerField()
    referred_match = OneToOneField(to=Match, on_delete=CASCADE, related_name="is_knockout_match")

    class Meta:
        verbose_name = "Knockout match"
        verbose_name_plural = "Knockout matches"

    def __str__(self):
        return self.turn.__str__() + ' : ' + self.referred_match.__str__()


class Goal(Model):
    scorer = ForeignKey(to=Footballer, on_delete=CASCADE, related_name="goals", null=False)
    assist_man = ForeignKey(null=True, blank=True, to=Footballer, on_delete=CASCADE, related_name="assists")
    match = ForeignKey(to=Match, on_delete=CASCADE, related_name="goals", null=False)
    minute = IntegerField(null=False)

    def __str__(self):
        string = self.match.__str__() + ' : ' + self.scorer.__str__()
        if self.assist_man is not None:
            string += " assisted by " + self.assist_man.__str__()
        string += " at " + self.minute.__str__() + "'"
        return string


class YellowCard(Model):
    admonished = ForeignKey(to=Footballer, on_delete=CASCADE, related_name="yellow_cards", null=False)
    minute = IntegerField(null=False)
    match = ForeignKey(to=Match, on_delete=CASCADE, related_name="yellow_cards", null=False)

    def get_class_name(self):
        return "YellowCard"

    def __str__(self):
        return self.match.__str__() + " : " + self.admonished.__str__() + " at minute " + self.minute.__str__()


class RedCard(Model):
    expelled = ForeignKey(to=Footballer, on_delete=CASCADE, related_name="red_cards", null=False)
    minute = IntegerField(null=False)
    match = ForeignKey(to=Match, on_delete=CASCADE, related_name="red_cards", null=False)
    double_yellow = BooleanField(null=False, default=False)

    def __str__(self):
        return self.match.__str__() + " : " + self.expelled.__str__() + " at minute " + self.minute.__str__()
