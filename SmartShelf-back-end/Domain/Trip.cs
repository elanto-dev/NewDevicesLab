using System;
using System.Collections.Generic;
using Domain.Identity;

namespace Domain
{
    public class Trip
    {
        public int Id { get; set; }
        public int UserId { get; set; }
        public User User { get; set; }
        public string Location { get; set; }
        public DateTime StartDateTime { get; set; }
        public DateTime EndDateTime { get; set; }
        public int? ReminderInAdvanceSeconds { get; set; }
        public bool UserLeftTheHouse { get; set; }
        public List<ShelfInTrip> TripSetups { get; set; }
    }
}
