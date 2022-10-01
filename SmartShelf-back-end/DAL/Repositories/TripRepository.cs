using System.Collections.Generic;
using System.Linq;
using System.Threading.Tasks;
using Domain;
using Microsoft.EntityFrameworkCore;
using Microsoft.EntityFrameworkCore.ChangeTracking;

namespace DAL.Repositories
{
    public class TripRepository : BaseRepository
    {
        private readonly AppDbContext _context;

        public TripRepository(AppDbContext context) : base(context)
        {
            _context = context;
        }

        public async Task<List<Trip>> GetByUserId(int userId)
        {
            return await _context.Trips.Where(t => t.UserId == userId).AsNoTracking().ToListAsync();
        }

        public async Task<Trip> GetById(int id)
        {
            return await _context.Trips.FirstOrDefaultAsync(t => t.Id == id);
        }

        public EntityEntry<Trip> Update(Trip trip)
        {
            return _context.Trips.Update(trip);
        }

        public async Task<EntityEntry<Trip>> AddAsync(Trip shelf)
        {
            return await _context.Trips.AddAsync(shelf);
        }

        public EntityEntry<Trip> Delete(Trip shelf)
        {
            return _context.Trips.Remove(shelf);
        }

        public async Task<bool> Exists(int id)
        {
            return await _context.Trips.AnyAsync(s => s.Id == id);
        }
    }
}
