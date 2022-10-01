using System.ComponentModel.DataAnnotations;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Domain.Identity;
using Identity;
using Microsoft.AspNetCore.Identity;
using Microsoft.Extensions.Configuration;

namespace WebApp.ApiController
{
    [Route("api/[controller]")]
    [ApiController]
    public class UsersController : ControllerBase
    {
        private readonly SignInManager<User> _signInManager;
        private readonly UserManager<User> _userManager;
        private readonly IConfiguration _configuration;


        public UsersController(SignInManager<User> signInManager, IConfiguration configuration, UserManager<User> userManager)
        {
            _signInManager = signInManager;
            _configuration = configuration;
            _userManager = userManager;
        }

        /// <summary>
        /// Login action that logs user in with required parameters.
        /// </summary>
        /// <param name="model">Login model includes email and password</param>
        /// <returns></returns>
        [HttpPost]
        public async Task<ActionResult<string>> Login([FromBody] LoginDTO model)
        {

            var appUser = await _userManager.FindByEmailAsync(model.Email);

            if (appUser == null)
            {
                return StatusCode(403);
            }

            // do not log user in, just check that the password is ok
            var result = await _signInManager.CheckPasswordSignInAsync(appUser, model.Password, false);

            if (result.Succeeded)
            {
                // create claims based user 
                var claimsPrincipal = await _signInManager.CreateUserPrincipalAsync(appUser);

                // get the Json Web Token
                var jwt = JwtHelper.GenerateJwt(
                    claimsPrincipal.Claims,
                    _configuration["JWT:Key"],
                    _configuration["JWT:Issuer"],
                    int.Parse(_configuration["JWT:ExpireDays"]));
                return Ok(new { token = jwt });
            }

            return StatusCode(403);
        }

        /// <summary>
        /// Register action creates a new user with given parameters.
        /// </summary>
        /// <param name="model">Register model with parameters</param>
        /// <returns></returns>
        [HttpPost]
        public async Task<ActionResult<string>> Register([FromBody] RegisterDTO model)
        {
            if (ModelState.IsValid)
            {
                var appUser = new User
                {
                    UserName = model.UserName,
                    Email = model.Email
                };
                var result = await _userManager.CreateAsync(appUser, model.Password);
                if (result.Succeeded)
                {
                    // create claims based user 
                    var claimsPrincipal = await _signInManager.CreateUserPrincipalAsync(appUser);

                    // get the Json Web Token
                    var jwt = JwtHelper.GenerateJwt(
                        claimsPrincipal.Claims,
                        _configuration["JWT:Key"],
                        _configuration["JWT:Issuer"],
                        int.Parse(_configuration["JWT:ExpireDays"]));
                    return Ok(new { token = jwt });

                }
                return StatusCode(406); //406 Not Acceptable
            }

            return StatusCode(400); //400 Bad Request
        }

        /// <summary>
        /// Login model with email and password parameters.
        /// </summary>
        public class LoginDTO
        {
            public string Email { get; set; }
            public string Password { get; set; }
        }

        /// <summary>
        /// Register model with username, last name, first name, email and password parameters.
        /// </summary>
        public class RegisterDTO
        {
            public string UserName { get; set; }

            [Required]
            public string Email { get; set; }

            [Required]
            [MinLength(6)]
            public string Password { get; set; }
        }


    }
}
